#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
#include <QFile>
#include <QByteArray>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslConfiguration>

#include "threadsocket.h"
#include "sqlinteract.h"

class LP;
class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(SqlInteract *db, LP* root, QObject *parent = 0);
    ~Server();

    void startServer(const QString &ip = "127.0.0.1", const quint16 &portNb = 50885);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    LP *_root;
    SqlInteract *_dataBase;
    std::map<qintptr,ThreadSocket*> _running_threads;
    QSslConfiguration _sslConfig;
};

#endif // SERVER_H
