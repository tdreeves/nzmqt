// Copyright 2011-2014 Johann Duscher (a.k.a. Jonny Dee). All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//    1. Redistributions of source code must retain the above copyright notice, this list of
//       conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright notice, this list
//       of conditions and the following disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY JOHANN DUSCHER ''AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those of the
// authors and should not be interpreted as representing official policies, either expressed
// or implied, of Johann Duscher.

#include "nzmqt.hpp"

#include <QDebug>
#include <QSocketNotifier>
#include <QTimer>
#include <climits>

namespace nzmqt
{

/*
 * ZMQMessage
 */

ZMQMessage::ZMQMessage()
    : super()
{
}

ZMQMessage::ZMQMessage(size_t size_)
    : super(size_)
{
}

ZMQMessage::ZMQMessage(void* data_, size_t size_, free_fn *ffn_, void* hint_)
    : super(data_, size_, ffn_, hint_)
{
}

ZMQMessage::ZMQMessage(const QByteArray& b)
    : super(size_t(b.size()))
{
    memcpy(data(), b.constData(), size_t(b.size()));
}

void ZMQMessage::move(ZMQMessage& msg_)
{
    super::move(static_cast<zmq::message_t&>(msg_));
}

#if 0
void ZMQMessage::clone(ZMQMessage* msg_)
{
    rebuild(msg_->size());
    memcpy(data(), msg_->data(), size());
}
#endif

QByteArray ZMQMessage::toByteArray()
{
    return size() <= INT_MAX ? QByteArray(data<char>(), int(size())) : QByteArray();
}



/*
 * ZMQSocket
 */

ZMQSocket::ZMQSocket(ZMQContext* context_, Type type_)
    : qsuper(nullptr)
    , zmqsuper(*context_, type_)
    , m_context(context_)
{
}

ZMQSocket::~ZMQSocket()
{
//    qDebug() << Q_FUNC_INFO << "Context:" << m_context;
    close();
}

void ZMQSocket::close()
{
//    qDebug() << Q_FUNC_INFO << "Context:" << m_context;
    if (m_context)
    {
        m_context->unregisterSocket(this);
        m_context = nullptr;
    }
    zmqsuper::close();
}

void ZMQSocket::setOption(Option optName_, const void *optionVal_, size_t optionValLen_)
{
    setsockopt(optName_, optionVal_, optionValLen_);
}

void ZMQSocket::setOption(Option optName_, const char* str_)
{
    setOption(optName_, str_, strlen(str_));
}

void ZMQSocket::setOption(Option optName_, const QByteArray& bytes_)
{
    setOption(optName_, bytes_.constData(), size_t(bytes_.size()));
}

void ZMQSocket::getOption(Option option_, void *optval_, size_t *optvallen_) const
{
    const_cast<ZMQSocket*>(this)->getsockopt(option_, optval_, optvallen_);
}

void ZMQSocket::bindTo(const QString& addr_)
{
    bind(addr_.toLocal8Bit());
}

void ZMQSocket::bindTo(const char *addr_)
{
    bind(addr_);
}

void ZMQSocket::unbindFrom(const QString& addr_)
{
    unbind(addr_.toLocal8Bit());
}

void ZMQSocket::unbindFrom(const char *addr_)
{
    unbind(addr_);
}

void ZMQSocket::connectTo(const QString& addr_)
{
    zmqsuper::connect(addr_.toLocal8Bit());
}

void ZMQSocket::connectTo(const char* addr_)
{
    zmqsuper::connect(addr_);
}

void ZMQSocket::disconnectFrom(const QString& addr_)
{
    zmqsuper::disconnect(addr_.toLocal8Bit());
}

void ZMQSocket::disconnectFrom(const char* addr_)
{
    zmqsuper::disconnect(addr_);
}

bool ZMQSocket::sendMessage(ZMQMessage& msg_, SendFlags flags_)
{
    return send(msg_, flags_);
}

bool ZMQSocket::sendMessage(const QByteArray& bytes_, SendFlags flags_)
{
    ZMQMessage msg(bytes_);
    return send(msg, flags_);
}

bool ZMQSocket::sendMessage(const QList<QByteArray>& msg_, SendFlags flags_)
{
    int i;
    for (i = 0; i < msg_.size() - 1; i++)
    {
        if (!sendMessage(msg_[i], flags_ | SND_MORE))
            return false;
    }
    if (i < msg_.size())
        return sendMessage(msg_[i], flags_);

    return true;
}

bool ZMQSocket::receiveMessage(ZMQMessage* msg_, ReceiveFlags flags_)
{
    return recv(msg_, flags_);
}

QList<QByteArray> ZMQSocket::receiveMessage(ReceiveFlags flags_)
{
    QList<QByteArray> parts;

    ZMQMessage msg;
    while (receiveMessage(&msg, flags_))
    {
        parts += msg.toByteArray();
        msg.rebuild();

        if (!hasMoreMessageParts())
            break;
    }

    return parts;
}

QList< QList<QByteArray> > ZMQSocket::receiveMessages(ReceiveFlags flags_)
{
    QList< QList<QByteArray> > ret;

    QList<QByteArray> parts = receiveMessage(flags_);
    while (!parts.isEmpty())
    {
        ret += std::move(parts);

        parts = receiveMessage(flags_);
    }

    return ret;
}

qintptr ZMQSocket::fileDescriptor() const
{
    qintptr value;
    size_t size = sizeof(value);
    getOption(OPT_FD, &value, &size);
    return value;
}

ZMQSocket::Events ZMQSocket::events() const
{
    qint32 value;
    size_t size = sizeof(value);
    getOption(OPT_EVENTS, &value, &size);
    return static_cast<Events>(value);
}

// Returns true if there are more parts of a multi-part message
// to be received.
bool ZMQSocket::hasMoreMessageParts() const
{
    qint32 value;
    size_t size = sizeof(value);
    getOption(OPT_RCVMORE, &value, &size);
    return value;
}

void ZMQSocket::setIdentity(const char* nameStr_)
{
    setOption(OPT_IDENTITY, nameStr_);
}

void ZMQSocket::setIdentity(const QString& name_)
{
    setOption(OPT_IDENTITY, name_.toLocal8Bit());
}

void ZMQSocket::setIdentity(const QByteArray& name_)
{
    setOption(OPT_IDENTITY, const_cast<char*>(name_.constData()), size_t(name_.size()));
}

QByteArray ZMQSocket::identity() const
{
    char idbuf[256];
    size_t size = sizeof(idbuf);
    getOption(OPT_IDENTITY, idbuf, &size);
    return QByteArray(idbuf, int(size));
}

void ZMQSocket::setLinger(int msec_)
{
    setOption(OPT_LINGER, msec_);
}

qint32 ZMQSocket::linger() const
{
    qint32 msec=-1;
    size_t size = sizeof(msec);
    getOption(OPT_LINGER, &msec, &size);
    return msec;
}

void ZMQSocket::subscribeTo(const char* filterStr_)
{
    setOption(OPT_SUBSCRIBE, filterStr_);
}

void ZMQSocket::subscribeTo(const QString& filter_)
{
    setOption(OPT_SUBSCRIBE, filter_.toLocal8Bit());
}

void ZMQSocket::subscribeTo(const QByteArray& filter_)
{
    setOption(OPT_SUBSCRIBE, filter_);
}

void ZMQSocket::unsubscribeFrom(const char* filterStr_)
{
    setOption(OPT_UNSUBSCRIBE, filterStr_);
}

void ZMQSocket::unsubscribeFrom(const QString& filter_)
{
    setOption(OPT_UNSUBSCRIBE, filter_.toLocal8Bit());
}

void ZMQSocket::unsubscribeFrom(const QByteArray& filter_)
{
    setOption(OPT_UNSUBSCRIBE, filter_);
}

void ZMQSocket::setSendHighWaterMark(int value_)
{
    setOption(OPT_SNDHWM, value_);
}

void ZMQSocket::setReceiveHighWaterMark(int value_)
{
    setOption(OPT_RCVHWM, value_);
}

bool ZMQSocket::isConnected()
{
    return const_cast<ZMQSocket*>(this)->connected();
}

QByteArray ZMQSocket::lastEndpoint() const
{
    char idbuf[1024] = "\0";
    size_t size = sizeof(idbuf);
    getOption(OPT_LAST_ENDPOINT, idbuf, &size);
    return QByteArray(idbuf);
}

/*
 * ZMQContext
 */

ZMQContext::ZMQContext(QObject* parent_, int io_threads_)
    : qsuper(parent_)
    , zmqsuper(io_threads_)
{
}

ZMQContext::~ZMQContext()
{
//    qDebug() << Q_FUNC_INFO << "Sockets:" << m_sockets;
    for(ZMQSocket* socket : registeredSockets())
    {
        socket->m_context = nullptr;
        // As stated by 0MQ, close() must ONLY be called from the thread
        // owning the socket. So we use 'invokeMethod' which (hopefully)
        // results in a 'close' call from within the socket's thread.
        QMetaObject::invokeMethod(socket, "close");
    }
}

ZMQSocket* ZMQContext::createSocket(ZMQSocket::Type type_, QObject* parent_)
{
    ZMQSocket* socket = createSocketInternal(type_);
    registerSocket(socket);
    socket->setParent(parent_);
    return socket;
}

void ZMQContext::registerSocket(ZMQSocket* socket_)
{
    m_sockets.push_back(socket_);
}

void ZMQContext::unregisterSocket(ZMQSocket* socket_)
{
    for(Sockets::iterator soIt = m_sockets.begin(); soIt != m_sockets.end(); ++soIt)
    {
        if (*soIt == socket_)
        {
            m_sockets.erase(soIt);
            break;
        }
    }
}

const ZMQContext::Sockets& ZMQContext::registeredSockets() const
{
    return m_sockets;
}

/*
 * SocketNotifierZMQSocket
 */

SocketNotifierZMQSocket::SocketNotifierZMQSocket(ZMQContext* context_, Type type_)
    : super(context_, type_)
    , socketNotifyRead_(nullptr)
{
    qintptr fd = fileDescriptor();

    socketNotifyRead_ = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    QObject::connect(socketNotifyRead_, &QSocketNotifier::activated, this, &SocketNotifierZMQSocket::socketReadActivity);
}

SocketNotifierZMQSocket::~SocketNotifierZMQSocket()
{
    close();
}

void SocketNotifierZMQSocket::close()
{
    socketNotifyRead_->deleteLater();
    super::close();
}

void SocketNotifierZMQSocket::socketReadActivity()
{
    socketNotifyRead_->setEnabled(false);

    try
    {
        while(isConnected() && (events() & EVT_POLLIN))
        {
            const QList<QByteArray> & message = receiveMessage();
            emit messageReceived(message);
        }
    }
    catch (const ZMQException& ex)
    {
        qWarning("Exception during read: %s", ex.what());
        emit notifierError(ex.num(), ex.what());
    }

    socketNotifyRead_->setEnabled(true);
}

/*
 * SocketNotifierZMQContext
 */

SocketNotifierZMQContext::SocketNotifierZMQContext(QObject* parent_, int io_threads_)
    : super(parent_, io_threads_)
{
}

SocketNotifierZMQSocket* SocketNotifierZMQContext::createSocketInternal(ZMQSocket::Type type_)
{
    SocketNotifierZMQSocket *socket = new SocketNotifierZMQSocket(this, type_);
    connect(socket, &SocketNotifierZMQSocket::notifierError, this, &SocketNotifierZMQContext::notifierError);
    return socket;
}

}
