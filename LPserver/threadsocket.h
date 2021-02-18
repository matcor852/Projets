#ifndef THREADSOCKET_H
#define THREADSOCKET_H

#include <QThread>
#include <QObject>
#include <QSslSocket>
#include <QDataStream>
#include <QList>
#include <QSslConfiguration>

#include "sqlinteract.h"

typedef qint32 packageSize;
typedef unsigned int uint;
typedef unsigned short int usint;

class LP;
class ThreadSocket : public QThread
{
    Q_OBJECT
public:
    explicit ThreadSocket(qintptr id, QSslConfiguration &config, SqlInteract *db, LP* root, QObject *parent = 0);
    virtual ~ThreadSocket();
    void run() override;
    void send(const QString message) const;

    enum _commands
    {
        accountCreation = 0,
        authentication  = 1,
        roomCreation    = 2,
        roomJoin        = 3,
        partyNextRound  = 15,
        errorOccured    = 100
    };

signals:
    void error(QSslSocket::SocketError &socketError) const;
    void finished(const qintptr ptr) const;
    void dispatch(const QString &msg) const;

private slots:
    void tcpReady();
    void sslError(const QList<QSslError> &errors) const;
    void disconnected();

private:
    void dispatcher(const QString &query);
    LP* _root;
    SqlInteract *_dataBase;
    QSslSocket *_ssl_socket;
    const QSslConfiguration *_sslConfig;
    const qintptr _socketDescriptor;
    packageSize _tailleMessage;
    const QString _primarySeparator     = "⏇∐⏈";
    const QString _secondarySeparator   = "⁑╬₼";
};

#endif // THREADSOCKET_H
