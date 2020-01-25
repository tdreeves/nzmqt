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

// --------------------------------------------------------------------------------
// Jan 2020 : https://github.com/tdreeves/nzmqt

// The original code is undergoing a fairly major rewrite at https://github.com/tdreeves/nzmqt
// This is work in progress and I have not yet done all I intend to do.
// Simplify, Simplify, Simplify.
// I removed all the thread based polling code. QSocketNotifier is the way to go.
// I removed QSocketNotifier::Write code as it was not needed and continually just consuming CPU.
// I will update with ZeroMQ's newer C++11 support and remove deprecation warnings.
// I will be adding radio/dish support.

// I will collapse the now not needed and confusing "SocketNotifierZMQSocket : public ZMQSocket" and
// "SocketNotifierZMQContext : public ZMQContext" class structure and just put everthing
// in ZMQSocket and ZMQContext.

// --------------------------------------------------------------------------------


#ifndef NZMQT_H
#define NZMQT_H

#include "nzmqt/nzmqt_export.h"

#include <zmq.hpp>

#include <QObject>
#include <QByteArray>
#include <QFlag>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QRunnable>
#include <QVector>

#include <type_traits>

class QSocketNotifier;

namespace nzmqt
{
    typedef zmq::free_fn free_fn;
    typedef zmq::pollitem_t pollitem_t;

    typedef zmq::error_t ZMQException;

    using zmq::poll;
    using zmq::version;

    // This class wraps ZMQ's message structure.
    class NZMQT_EXPORT ZMQMessage : private zmq::message_t
    {
        friend class ZMQSocket;

        typedef zmq::message_t super;

    public:
        ZMQMessage();

        ZMQMessage(size_t size_);

        ZMQMessage(void* data_, size_t size_, free_fn *ffn_, void* hint_ = nullptr);

        ZMQMessage(const QByteArray& b);

        using super::rebuild;

        void move(ZMQMessage& msg_);

        using super::copy;

        using super::more;

        //void clone(ZMQMessage* msg_);

        using super::data;

        using super::size;

        QByteArray toByteArray();
    };

    class ZMQContext;

    // This class cannot be instantiated. Its purpose is to serve as an
    // intermediate base class that provides Qt-based convenience methods
    // to subclasses.
    class NZMQT_EXPORT ZMQSocket : public QObject, private zmq::socket_t
    {
        Q_OBJECT
        Q_ENUMS(Type Event SendFlag ReceiveFlag Option)
        Q_FLAGS(Event Events)
        Q_FLAGS(SendFlag SendFlags)
        Q_FLAGS(ReceiveFlag ReceiveFlags)

        typedef QObject qsuper;
        typedef zmq::socket_t zmqsuper;

    public:
        enum Type
        {
            TYP_PAIR = ZMQ_PAIR,
            TYP_PUB = ZMQ_PUB,
            TYP_SUB = ZMQ_SUB,
            TYP_REQ = ZMQ_REQ,
            TYP_REP = ZMQ_REP,
            TYP_DEALER = ZMQ_DEALER,
            TYP_ROUTER = ZMQ_ROUTER,
            TYP_PULL = ZMQ_PULL,
            TYP_PUSH = ZMQ_PUSH,
            TYP_XPUB = ZMQ_XPUB,
            TYP_XSUB = ZMQ_XSUB
        };

        enum Event : int
        {
            EVT_POLLIN = ZMQ_POLLIN,
            EVT_POLLOUT = ZMQ_POLLOUT,
            EVT_POLLERR = ZMQ_POLLERR
        };
        Q_DECLARE_FLAGS(Events, Event)

        enum SendFlag : int
        {
            SND_MORE = ZMQ_SNDMORE,
            SND_DONTWAIT = ZMQ_DONTWAIT
        };
        Q_DECLARE_FLAGS(SendFlags, SendFlag)

        enum ReceiveFlag : int
        {
            RCV_DONTWAIT = ZMQ_DONTWAIT
        };
        Q_DECLARE_FLAGS(ReceiveFlags, ReceiveFlag)

        enum Option
        {
            // Get only.
            OPT_TYPE = ZMQ_TYPE,
            OPT_RCVMORE = ZMQ_RCVMORE,
            OPT_FD = ZMQ_FD,
            OPT_EVENTS = ZMQ_EVENTS,
            OPT_MAXMSGSIZE = ZMQ_MAXMSGSIZE,
            OPT_LAST_ENDPOINT = ZMQ_LAST_ENDPOINT,

            // Set only.
            OPT_SUBSCRIBE = ZMQ_SUBSCRIBE,
            OPT_UNSUBSCRIBE = ZMQ_UNSUBSCRIBE,
            OPT_IMMEDIATE = ZMQ_IMMEDIATE,
            OPT_REQ_CORRELATE = ZMQ_REQ_CORRELATE,
            OPT_REQ_RELAXED = ZMQ_REQ_RELAXED,

            // Get and set.
            OPT_AFFINITY = ZMQ_AFFINITY,
            OPT_IDENTITY = ZMQ_IDENTITY,
            OPT_RATE = ZMQ_RATE,
            OPT_RECOVERY_IVL = ZMQ_RECOVERY_IVL,
            OPT_SNDBUF = ZMQ_SNDBUF,
            OPT_RCVBUF = ZMQ_RCVBUF,
            OPT_LINGER = ZMQ_LINGER,
            OPT_RECONNECT_IVL = ZMQ_RECONNECT_IVL,
            OPT_RECONNECT_IVL_MAX = ZMQ_RECONNECT_IVL_MAX,
            OPT_BACKLOG = ZMQ_BACKLOG,
            OPT_SNDHWM = ZMQ_SNDHWM,
            OPT_RCVHWM = ZMQ_RCVHWM,
            OPT_SNDTIMEO = ZMQ_SNDTIMEO,
            OPT_RCVTIMEO = ZMQ_RCVTIMEO,
            OPT_IPV6 = ZMQ_IPV6,
            OPT_CONFLATE = ZMQ_CONFLATE,
            OPT_TOS = ZMQ_TOS,
        };

        ~ZMQSocket();

        using zmqsuper::operator void *;

        void setOption(Option optName_, const void *optionVal_, size_t optionValLen_);

        void setOption(Option optName_, const char* str_);

        void setOption(Option optName_, const QByteArray& bytes_);

        template<typename INT_T, typename = typename std::enable_if<std::is_integral<INT_T>::value>::type>
        void setOption(Option optName_, INT_T value_)
        {
            setOption(optName_, &value_, sizeof(value_));
        }

        void getOption(Option option_, void *optval_, size_t *optvallen_) const;

        void bindTo(const QString& addr_);

        void bindTo(const char *addr_);

        void unbindFrom(const QString& addr_);

        void unbindFrom(const char *addr_);

        void connectTo(const QString& addr_);

        void connectTo(const char* addr_);

        void disconnectFrom(const QString& addr_);

        void disconnectFrom(const char* addr_);

        bool sendMessage(ZMQMessage& msg_, SendFlags flags_ = SND_DONTWAIT);

        // Receives a message or a message part.
        bool receiveMessage(ZMQMessage* msg_, ReceiveFlags flags_ = RCV_DONTWAIT);

        // Receives a message.
        // The message is represented as a list of byte arrays representing
        // a message's parts. If the message is not a multi-part message the
        // list will only contain one array.
        QList<QByteArray> receiveMessage(ReceiveFlags flags_ = RCV_DONTWAIT);

        // Receives all messages currently available.
        // Each message is represented as a list of byte arrays representing the messages
        // and their parts in case of multi-part messages. If a message isn't a multi-part
        // message the corresponding byte array list will only contain one element.
        // Note that this method won't work with REQ-REP protocol.
        QList< QList<QByteArray> > receiveMessages(ReceiveFlags flags_ = RCV_DONTWAIT);

        qintptr fileDescriptor() const;

        Events events() const;

        // Returns true if there are more parts of a multi-part message
        // to be received.
        bool hasMoreMessageParts() const;

        void setIdentity(const char* nameStr_);

        void setIdentity(const QString& name_);

        void setIdentity(const QByteArray& name_);

        QByteArray identity() const;

        void setLinger(int msec_);

        qint32 linger() const;

        void subscribeTo(const char* filterStr_);

        void subscribeTo(const QString& filter_);

        void subscribeTo(const QByteArray& filter_);

        void unsubscribeFrom(const char* filterStr_);

        void unsubscribeFrom(const QString& filter_);

        void unsubscribeFrom(const QByteArray& filter_);

        void setSendHighWaterMark(int value_);

        void setReceiveHighWaterMark(int value_);

        bool isConnected();

        QByteArray lastEndpoint() const;

    signals:
        void messageReceived(const QList<QByteArray>&);

    public slots:
        void close();

        // Send the given bytes as a single-part message.
        bool sendMessage(const QByteArray& bytes_, nzmqt::ZMQSocket::SendFlags flags_ = SND_DONTWAIT);

        // Interprets the provided list of byte arrays as a multi-part message
        // and sends them accordingly.
        // If an empty list is provided this method doesn't do anything and returns trua.
        bool sendMessage(const QList<QByteArray>& msg_, nzmqt::ZMQSocket::SendFlags flags_ = SND_DONTWAIT);


    protected:
        ZMQSocket(ZMQContext* context_, Type type_);

    private:
        friend class ZMQContext;

        ZMQContext* m_context;
    };
    Q_DECLARE_OPERATORS_FOR_FLAGS(ZMQSocket::Events)
    Q_DECLARE_OPERATORS_FOR_FLAGS(ZMQSocket::SendFlags)
    Q_DECLARE_OPERATORS_FOR_FLAGS(ZMQSocket::ReceiveFlags)


    class SocketNotifierZMQSocket;

    // This class is an abstract base class for concrete implementations.
    class NZMQT_EXPORT ZMQContext : public QObject, private zmq::context_t
    {
        Q_OBJECT

        typedef QObject qsuper;
        typedef zmq::context_t zmqsuper;

        friend class ZMQSocket;

    public:
        ZMQContext(QObject* parent_ = nullptr, int io_threads_ = 1);

        // Deleting children is necessary, because otherwise the children are deleted after the context
        // which results in a blocking state. So we delete the children before the zmq::context_t
        // destructor implementation is called.
        ~ZMQContext();

        using zmqsuper::operator void*;

        // Creates a socket instance of the specified type and parent.
        // The created instance will have the specified parent
        // (as usual you can also call 'ZMQSocket::setParent()' method to change
        // ownership later on). Make sure, however, that the socket's parent
        // belongs to the same thread as the socket instance itself (as it is required
        // by Qt). Otherwise, you will encounter strange errors.
        ZMQSocket* createSocket(ZMQSocket::Type type_, QObject* parent_ = nullptr);

    protected:
        typedef QVector<ZMQSocket*> Sockets;

        // Creates a socket instance of the specified type.
        virtual ZMQSocket* createSocketInternal(ZMQSocket::Type type_) = 0;

        virtual void registerSocket(ZMQSocket* socket_);

        // Remove the given socket object from the list of registered sockets.
        virtual void unregisterSocket(ZMQSocket* socket_);

        virtual const Sockets& registeredSockets() const;

    private:
        Sockets m_sockets;
    };

    // An instance of this class cannot directly be created. Use one
    // of the 'SocketNotifierZMQContext::createSocket()' factory methods instead.
    class NZMQT_EXPORT SocketNotifierZMQSocket : public ZMQSocket
    {
        Q_OBJECT

        friend class SocketNotifierZMQContext;

        typedef ZMQSocket super;

    public:
        using super::close;

        void close();

    signals:
        // This signal will be emitted by the socket notifier callback if a call
        // to the events() method results in an exception.
        void notifierError(int errorNum, const QString& errorMsg);

    protected:
        SocketNotifierZMQSocket(ZMQContext* context_, Type type_);
        ~SocketNotifierZMQSocket();

    protected slots:
        void socketReadActivity();

    private:
        QSocketNotifier *socketNotifyRead_;
    };

    class NZMQT_EXPORT SocketNotifierZMQContext : public ZMQContext
    {
        Q_OBJECT

        typedef ZMQContext super;

    public:
        SocketNotifierZMQContext(QObject* parent_ = nullptr, int io_threads_ = 1);

    signals:
        // This signal will be emitted by the socket notifier callback if a call
        // to the events() method results in an exception.
        void notifierError(int errorNum, const QString& errorMsg);

    protected:
        SocketNotifierZMQSocket* createSocketInternal(ZMQSocket::Type type_) override;
    };

}

// Declare metatypes for using them in Qt signals.
Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QList< QList<QByteArray> >)
Q_DECLARE_METATYPE(nzmqt::ZMQSocket::SendFlags)

#endif // NZMQT_H
