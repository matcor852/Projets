#include "threadsocket.h"
#include "lp.h"

ThreadSocket::ThreadSocket(qintptr id, QSslConfiguration &config, SqlInteract *db,
                           LP* root, QObject *parent)
    : QThread(parent), _root(root), _dataBase(db), _sslConfig(&config), _socketDescriptor(id)
{

}

ThreadSocket::~ThreadSocket()
{
    if(!this->isFinished())
    {
        qDebug() << "thread n°" << _socketDescriptor << " ordered deletion but still running";
        //_root->writeLogs("thread n°" + QString::number(_socketDescriptor) + " ordered deletion but still running");
        this->quit();
        this->wait();
    }
    _root->removePlayer(_ssl_socket);
    _root->removeClient(_ssl_socket);
    _ssl_socket->deleteLater();
}

void ThreadSocket::run()
{
    _ssl_socket = new QSslSocket;
    _ssl_socket->setSslConfiguration(*_sslConfig);
    if(!_ssl_socket->setSocketDescriptor(_socketDescriptor))
    {
        qDebug() << "error assigning descriptor  " + QString::number(_socketDescriptor) + "; exiting...";
        _root->writeLogs("error assigning descriptor  " + QString::number(_socketDescriptor) + "; exiting...");
        return disconnected();
    }
    connect(_ssl_socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this, &ThreadSocket::sslError);
    connect(_ssl_socket, &QSslSocket::readyRead, this, &ThreadSocket::tcpReady, Qt::DirectConnection);
    connect(_ssl_socket, &QSslSocket::disconnected, this, &ThreadSocket::disconnected);
    connect(this, &ThreadSocket::dispatch, this, &ThreadSocket::dispatcher);
    _ssl_socket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    _ssl_socket->startServerEncryption();
    _root->addClient(_ssl_socket);
    exec();
}

void ThreadSocket::send(const QString message) const
{
    if(!_ssl_socket->isEncrypted())
    {
        qDebug() << "socket n°" << _socketDescriptor << " unencrypted; sending attempt failed.";
        _root->writeLogs("socket n°" + QString::number(_socketDescriptor) + " unencrypted; sending attempt failed.");
        return;
    }
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out << (packageSize) 0;
    out << message;//.toUtf8();
    out.device()->seek(0);
    out << (packageSize) (paquet.size() - sizeof (packageSize));
    _ssl_socket->write(paquet);
}

void ThreadSocket::tcpReady()
{
    if(_ssl_socket == 0 || !_ssl_socket->isEncrypted())
        return;
    QDataStream in(_ssl_socket);
    if(_tailleMessage == 0)
    {
        if(_ssl_socket->bytesAvailable() < (packageSize)sizeof(packageSize))
        {
            return;
        }
        in >> _tailleMessage;
    }
    if(_ssl_socket->bytesAvailable() < _tailleMessage)
        return;
    QString message;
    in >> message;
    _tailleMessage = 0;
    emit dispatch(message);
}

void ThreadSocket::sslError(const QList<QSslError> &errors) const
{
    QString erroStr = "";
    for(const QSslError &e : errors)
    {
        erroStr.append(e.errorString()).append("\t");
    }
    qDebug() << erroStr;
    _root->writeLogs("socket n°"+QString::number(_socketDescriptor)+" ssl error(s) : "+erroStr);
    //_ssl_socket->ignoreSslErrors();
}

void ThreadSocket::disconnected()
{
    _ssl_socket->abort();
    quit();
    wait();
    emit finished(_socketDescriptor);
}

void ThreadSocket::dispatcher(const QString &query)
{
    if(query.count(_primarySeparator) != 1)
    {
        qDebug() << _ssl_socket << "error separator '"+_primarySeparator+"' for query : "+query+"; expected 1";
        _root->writeLogs("socket n°"+QString::number(_socketDescriptor)+" error separator '"+_primarySeparator+"' for query : "+query+"; expected 1");
        return;
    }
    const _commands command = (_commands)query.split(_primarySeparator).first().toUInt();
    switch(command)
    {
        case _commands::accountCreation:        /// 0⏇∐⏈nom⁑╬₼prenom⁑╬₼age⁑╬₼username⁑╬₼genre⁑╬₼password⁑╬₼hostname
        {
            qDebug() << "in accountCreation";
            if(query.split(_primarySeparator).at(1).count(_secondarySeparator) != 6)
            {
                qDebug() << _ssl_socket << "error separator '"+_secondarySeparator+"' in : "+query+"; 6 expected";
                _root->writeLogs("socket n°"+QString::number(_socketDescriptor)+" error separator '"+_secondarySeparator+"' in : "+query+"; 6 expected");
                return;
            }
            const QStringList var = query.split(_primarySeparator).at(1).split(_secondarySeparator);
            const std::pair<bool, QString> result = _dataBase->CreateUser(var.first(), var.at(1), var.at(2), var.at(3), var.at(4), var.at(5), var.last());
            if(!result.first)
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+result.second);
                return;
            }
            send(QString::number(_commands::accountCreation)+_primarySeparator+result.second);
            break;
        }
        case _commands::authentication:         /// 1⏇∐⏈username⁑╬₼password⁑╬₼playerIcon
        {
            qDebug() << "in authent";
            _root->writeLogs("in authent");
            if(query.split(_primarySeparator).at(1).count(_secondarySeparator) != 2)
            {
                qDebug() << _ssl_socket << "error separator '"+_secondarySeparator+"' in : "+query+"; 2 expected";
                _root->writeLogs("socket n°"+QString::number(_socketDescriptor)+" error separator '"+_secondarySeparator+"' in : "+query+"; 2 expected");
                return;
            }
            const QStringList var = query.split(_primarySeparator).at(1).split(_secondarySeparator);
            const std::pair<bool, QString> result = _dataBase->Identify(var.first(), var.at(1));
            if(!result.first)
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+"Echec de l'authentification : " + result.second);
                return;
            }
            if(_root->userConnected(var.first(), var.last()))
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+"Utilisateur déjà connecté");
                return;
            }
            _ssl_socket->setObjectName(var.first());
            qDebug() << var.first()+" authented";
            send(QString::number(_commands::authentication)+_primarySeparator+"true");
            break;
        }
        case _commands::roomCreation:
        {
            if(_ssl_socket->objectName().isEmpty())
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+"Unauthentified user; cannot create room");
                return;
            }
            const usint nbPartie = _root->createRoom(_ssl_socket);
            if(!nbPartie)
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+"maximum games capacity reached on server");
                return;
            }
            send(QString::number(_commands::roomCreation)+_primarySeparator+QString::number(nbPartie));
            break;
        }
        case _commands::roomJoin:
        {
            if(_ssl_socket->objectName().isEmpty())
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+"Unauthentified user; cannot join room");
                return;
            }
            const usint nbPartie = query.split(_primarySeparator).at(1).toUInt();
            const std::pair<bool, QString> result = _root->joinRoom(nbPartie, _ssl_socket);
            if(!result.first)
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+result.second);
                return;
            }
            break;
        }
        case _commands::partyNextRound:
        {
            if(_ssl_socket->objectName().isEmpty())
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+"Unauthentified user; cannot interact with room");
                return;
            }
            const usint nbPartie = query.split(_primarySeparator).at(1).toUInt();
            const std::pair<bool, QString> result = _root->nextRound(nbPartie);
            if(!result.first)
            {
                send(QString::number(_commands::errorOccured)+_primarySeparator+result.second);
                return;
            }
            break;
        }
    default:
        send(QString::number(_commands::errorOccured)+_primarySeparator+"Unknown command");
    }
}
