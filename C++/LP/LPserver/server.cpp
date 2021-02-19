#include "server.h"
#include "lp.h"

Server::Server(SqlInteract *db, LP* root, QObject *parent)
    : QTcpServer(parent), _root(root), _dataBase(db)
{
#ifdef Q_OS_WINDOWS
    const QString key_path = /*windows pem private key path*/;
    const QString cert_path = /*windows pem certificate path*/;
#else
    const QString key_path = /*unix pem private key path*/;
    const QString cert_path = /*unix pem certificate path*/;
#endif
    QByteArray key;
    QByteArray cert;
    QFile file_key(key_path);
    if(file_key.open(QIODevice::ReadOnly))
    {
        key = file_key.readAll();
        file_key.close();
    }
    else
    {
        qDebug() << file_key.errorString();
        _root->writeLogs("\terror loading server key : "+file_key.errorString());
    }

    QFile file_cert(cert_path);
    if(file_cert.open(QIODevice::ReadOnly))
    {
        cert = file_cert.readAll();
        file_cert.close();
    }
    else
    {
        qDebug() << file_cert.errorString();
        _root->writeLogs("\terror loading server certificate : "+file_cert.errorString());
    }

    QSslKey ssl_key(key, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    QSslCertificate ssl_cert(cert);
    _sslConfig.addCaCertificate(ssl_cert);
    _sslConfig.setLocalCertificate(ssl_cert);
    _sslConfig.setPrivateKey(ssl_key);
    _sslConfig.setProtocol(QSsl::TlsV1_3);
    _sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    _root->setRunningThreads(&_running_threads);
}

Server::~Server()
{
    for(auto pair : _running_threads)
    {
        pair.second->deleteLater();
    }
    _running_threads.clear();
}

void Server::startServer(const QString &ip, const quint16 &portNb)
{
    if(!this->listen(QHostAddress(ip), portNb))
    {
        qDebug() << "server start failed";
        _root->writeLogs("\tserver start failed; ip = "+ip+", port = "+QString::number(portNb)+" ("+errorString()+")");
    }
    qDebug() << "server " + ip + " listening on port " + QString::number(portNb);
    _root->writeLogs("server start succeeded; ip = "+ip+", port = "+QString::number(portNb));
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    _running_threads[socketDescriptor] = new ThreadSocket(socketDescriptor, _sslConfig, _dataBase, _root, _root);
    connect(_running_threads.at(socketDescriptor), &ThreadSocket::finished, [this](const qintptr ptr){this->_running_threads.at(ptr)->deleteLater();});
    _running_threads.at(socketDescriptor)->start();
}
