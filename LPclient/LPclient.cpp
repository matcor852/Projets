#include "LPclient.h"

LPclient::LPclient(QObject *parent) : QObject(parent)
{
    _socket = new QSslSocket(this);
    _socket->setProtocol(QSsl::TlsV1_3);
    _socket->setPeerVerifyMode(QSslSocket::VerifyPeer);

    _socket->setProxy(QNetworkProxy::NoProxy);
    connect(_socket, &QSslSocket::encrypted, this, &LPclient::connecte);
    connect(_socket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors), this, &LPclient::sslErrors);
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));
    connect(_socket, &QSslSocket::disconnected, this, &LPclient::deconnecte);
    connect(_socket, &QSslSocket::readyRead, this, &LPclient::dataReceived);
    _tailleMessage = 0;

    _connexionTimer = new QTimer(this);
    connect(_connexionTimer, &QTimer::timeout, [this]{send(QString::number(_commands::authentication)%
                                                           _primarySeparator%_username%
                                                           _secondarySeparator%_password%
                                                           _secondarySeparator%_playersIcon[_username]);});
}

LPclient::~LPclient()
{
    if(_socket->isOpen())
    {
        _socket->abort();
        _socket->close();
    }
    delete _connexionTimer;
    delete _socket;
}

void LPclient::login(const QString &username, const QString &password, const QVariant iconPath)
{
    _username = username;
    _password = passwordHash(password);
    QFile file(iconPath.toString().remove("file:///"));
    if(!file.open(QIODevice::ReadOnly))
    {
        emit appendInfos("<font color='red'>Erreur lors de la lecture de l'image : "+file.errorString()+"</font>");
        return;
    }
    const QString fileData = file.readAll().toBase64();
    file.close();
    if(fileData.size() > 4*pow(10,6))
    {
        emit appendInfos("<font color='red'>Icon trop lourd : "+QString::number(fileData.size()*pow(10, -6))+" Mo ! (taille supérieure à 4Mo)</font>");
        return;
    }
    _playersIcon[_username] = fileData;
    emit appendInfos(QString("<font color='white'><em>Connexion...</em></font>"));
    _socket->abort();
    _socket->connectToHostEncrypted(_ip, _port);
}

const QString LPclient::passwordHash(const QString &password) const
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_256);
}

void LPclient::askAccountCreation(const QString &nom, const QString &prenom, const unsigned &age, const QString &pseudo, const QString &genre, const QString &password) const
{
    if(age < 18)
    {
        emit appendInfos("<font color='red'><strong> Vous devez avoir au moins 18 ans pour vous inscrire ! </strong></font>");
        return;
    }
    if(password.length() < 6 || password.length() > 32)
    {
        emit appendInfos("<font color='red'><strong> Le mot de passe doit faire entre 6 et 32 caractères ! </strong></font>");
        return;
    }
    if(pseudo.length() < 3 || pseudo.length() > 15)
    {
        emit appendInfos("<font color='red'><strong> Le pseudo doit faire entre 3 et 15 caractères ! </strong></font>");
        return;
    }
    disconnect(_socket, &QSslSocket::encrypted, this, &LPclient::connecte);
    _socket->abort();
    _socket->connectToHostEncrypted(_ip, _port);
    if(_socket->waitForEncrypted(10000))
    {
        if(_socket->isEncrypted())
            send(QString::number(_commands::accountCreation)%_primarySeparator%nom%_secondarySeparator
                 %prenom%_secondarySeparator%QString::number(age)%_secondarySeparator%pseudo%_secondarySeparator
                 %genre%_secondarySeparator%passwordHash(password)%_secondarySeparator%QHostInfo::localHostName());
        else
            emit appendInfos("<font color='red'><em>Un problème est survenu, vérifiez votre connexion et réessayer.</em></font>");
    }
    connect(_socket, &QSslSocket::encrypted, this, &LPclient::connecte);
}

void LPclient::askCreate() const
{
    send(QString::number(_commands::roomCreation)%_primarySeparator%"create");
}

void LPclient::askJoin(const unsigned int &partyNb) const
{
    send(QString::number(_commands::roomJoin)%_primarySeparator%QString::number(partyNb));
}

void LPclient::askNext() const
{
    send(QString::number(_commands::partyNextRound)%_primarySeparator%QString::number(_nbParty));
}

void LPclient::askQuit() const
{
    _socket->abort();
    emit swiper(1);
}

void LPclient::dataReceived()
{
    if(_socket == 0 || !_socket->isEncrypted())
        return;
    QDataStream in(_socket);
    if(_tailleMessage==0)
    {
        if(_socket->bytesAvailable() < (packageSize)sizeof(packageSize))
            return;
        in >> _tailleMessage;
    }
    if(_socket->bytesAvailable() < _tailleMessage)
        return;
    QString resp;
    in >> resp;
    _tailleMessage = 0;
    allocateOrders(resp);
}

void LPclient::connecte() const
{
    emit appendInfos("<font color='green'><em>Connexion réussie</em></font>");
    emit appendInfos("<font color='white'>Authentification...</font>");
    _connexionTimer->start(500);
}

void LPclient::deconnecte() const
{
    _connexionTimer->stop();
    _socket->abort();
    emit this->appendInfos("<font color='red'><em>Déconnecté du serveur</em></font>");
    emit this->swiper(1);
}

void LPclient::erreurSocket(const QAbstractSocket::SocketError &erreur) const
{
    QString dpl = "";
    switch(erreur)
    {
        case QAbstractSocket::HostNotFoundError:
            dpl.append("ERREUR : le serveur n'a pas pu être trouvé");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            dpl.append("ERREUR : le serveur a refusé la connection");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            dpl.append("ERREUR : le serveur a coupé la connexion");
            break;
        default:
            dpl.append(QString("ERREUR : %1").arg(_socket->errorString()));
    }
    emit appendInfos("<font color='red'>"+dpl+"</font>");
}

void LPclient::sslErrors(const QList<QSslError> &errors) const
{
    for(const auto &error : errors)
    {
        emit appendInfos(error.errorString());
    }
    //_socket->ignoreSslErrors(errors);
}

void LPclient::allocateOrders(const QString &order)
{
    if(order.count(_primarySeparator) == 1)
    {
        _commands command = (_commands)order.split(_primarySeparator)[0].toInt();
        switch(command)
        {
            case _commands::accountCreation:
            {
                emit swiper(1);
                emit appendInfos("<font color='green'>Le compte a été créé avec succès ! ("+order.split(_primarySeparator)[1]+")</font>");
                break;
            }
            case _commands::authentication:
            {
                _connexionTimer->stop();
                emit appendInfos("<font color='green'>Utilisateur authentifié</font>");
                emit authented();
                break;
            }
            case _commands::roomCreation:
            {
                _nbParty = order.split(_primarySeparator)[1].toUInt();
                emit integrated(_nbParty);
                emit appendInfos("<font color='green'>Partie créée : "+QString::number(_nbParty)+"</font>");
                break;
            }
            case _commands::roomJoin:
            {
                _nbParty = order.split(_primarySeparator)[1].toUInt();
                emit integrated(_nbParty);
                emit appendInfos("<font color='green'>Partie intégrée : "+QString::number(_nbParty)+"</font>");
                break;
            }
            case _commands::playersList:
            {
                bool cleanList = true;
                if(order.split(_primarySeparator).at(1).count(_playersSeparator))
                {
                    _playersIcon.clear();
                    for(const QString &playerData : order.split(_primarySeparator).at(1).split(_playersSeparator))
                    {
                        if(playerData.count(_playerInfosSeparator) != 1)
                            continue;
                        const QStringList data = playerData.split(_playerInfosSeparator);
                        _playersIcon[data.first()] = data.last();
                        emit refreshPlayers(_playersIcon[data.first()],data.first(),cleanList);
                        cleanList = false;
                    }
                }
                else
                {
                    if(order.split(_primarySeparator).at(1).count(_playerInfosSeparator) != 1)
                        return;
                    _playersIcon.clear();
                    const QStringList data = order.split(_primarySeparator).at(1).split(_playerInfosSeparator);
                    _playersIcon[data.first()] = data.last();
                    emit refreshPlayers(_playersIcon[data.first()],data.first(),cleanList);
                }
                break;
            }
            case _commands::partyNextRound:
            {
                if(order.split(_primarySeparator).at(1).count(_secondarySeparator) != 1)
                {
                    emit appendInfos("<font color='red'>ERREUR : séparateur manquant '"+_secondarySeparator+"' !</font>");
                    return;
                }
                const QString player = order.split(_primarySeparator).at(1).split(_secondarySeparator).first();
                const QString gage = order.split(_primarySeparator).at(1).split(_secondarySeparator).last();
                emit newAction((_playersIcon.count(player))?_playersIcon.at(player):"", player,gage);
                break;
            }
            case _commands::inheritParty:
            {
                emit appendInfos("<font color='white'>inherited party ownership</font>");
                emit inheritedParty(true);
                break;
            }
            case _commands::error:
            {
                emit appendInfos("<font color='red'>"+order.split(_primarySeparator)[1]+"</font>");
                break;
            }
            default:
                qDebug() << "<font color='red'>ERROR: Unknown command</font>";
        }
    }
}

void LPclient::send(const QString msg) const
{
    if(!_socket->isOpen() || !_socket->isEncrypted())
    {
        emit appendInfos("<font color='red'>Connexion non établie/chiffrée</font>");
        emit swiper(1);
        return;
    }
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);
    out << (packageSize) 0;
    out << msg;
    out.device()->seek(0);
    out << (packageSize) (paquet.size() - sizeof(packageSize));
    _socket->write(paquet);
    //_socket->flush();
}
