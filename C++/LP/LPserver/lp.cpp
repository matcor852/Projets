#include "lp.h"

LP::LP(QObject *parent)
    : QObject(parent), _mutexLp()
{

#ifdef Q_OS_WINDOWS
    const QString dbPath = /*windows database path*/;
#else
    const QString dbPath = /*unix database path*/;
#endif

    _fileLogs.setFileName("logs_"+QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ss")+".txt");
    if(!_fileLogs.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "error creating log file : " << _fileLogs.errorString();
    }
    _fileLogs.close();
    _dataBase = new SqlInteract(this, dbPath);
    connect(_dataBase, &SqlInteract::debugInfos, this, &LP::writeLogs);
    _nbDare = _dataBase->nbGages("dare");
    _nbTruth = _dataBase->nbGages("truth");
    _tcpServer = new Server(_dataBase, this, this);
    _tcpServer->startServer(_ipTcp, _portTcp);
}

LP::~LP()
{
    _playersIcon.clear();
    _connectedSockets.clear();
    _belongingMap.clear();
    for(auto pair : _rooms)
    {
        pair.second->deleteLater();
    }
    _rooms.clear();
    _fileLogs.close();
    delete _tcpServer;
    delete _dataBase;
}

void LP::addClient(QSslSocket *client)
{
    QMutexLocker locker(&_mutexLp);
    _connectedSockets.append(client);
}

void LP::removeClient(QSslSocket *client)
{
    QMutexLocker locker(&_mutexLp);
    _connectedSockets.removeOne(client);
    if(_playersIcon.count(client->objectName()))
        _playersIcon.erase(client->objectName());
}

bool LP::roomExist(const usint nPartie) const
{
    if(_rooms.count(nPartie))
        return true;
    else
        return false;
}

void LP::removePlayer(QSslSocket *client)
{
    QMutexLocker locker(&_mutexLp);
    if(_belongingMap.count(client))
    {
        _rooms.at(_belongingMap.at(client))->delPlayer(client);
        _belongingMap.erase(client);
    }
}

bool LP::userConnected(const QString username, const QString imgData)
{
    QMutexLocker locker(&_mutexLp);
    for(QSslSocket *soc : _connectedSockets)
    {
        if(soc->objectName() == username)
            return true;
    }
    _playersIcon[username] = imgData;
    return false;
}

usint LP::createRoom(QSslSocket *client)
{
    QMutexLocker locker(&_mutexLp);
    usint nbPartie = 0;
    if(_rooms.size() < 2000)
    {
        do {
            nbPartie = rand()%2000+2000;
        } while(roomExist(nbPartie));
        _rooms[nbPartie] = new Party(client, _nbDare, _nbTruth, _dataBase, this);
        _rooms.at(nbPartie)->setObjectName(QString::number(nbPartie));
        _belongingMap[client] = nbPartie;
        connect(_rooms.at(nbPartie), &Party::altered, this, &LP::partyAltered);
        connect(_rooms.at(nbPartie), &Party::deletedRoom, this, &LP::eraseParty);
        emit newRoom(nbPartie);
    }
    return nbPartie;
}

void LP::eraseParty(const usint nPartie)
{
    QMutexLocker locker(&_mutexLp);
    if(roomExist(nPartie))
    {
        _rooms.erase(nPartie);
    }
    emit lessRoom(nPartie);
}

usint LP::nbPlayers(const usint nbPartie) const
{
    if(!roomExist(nbPartie))
        return 0;
    return _rooms.at(nbPartie)->nbPlayers();
}

const std::pair<bool, QString> LP::joinRoom(const usint nbPartie,  QSslSocket *client)
{
    QMutexLocker locker(&_mutexLp);
    const usint nb_players = nbPlayers(nbPartie);
    if(nb_players == 0)
        return {false, "uncreated room"};
    else if(nb_players >= 15)
        return {false, "capacité maximale atteinte (15 joueurs)"};
    _rooms.at(nbPartie)->addPlayer(client);
    _belongingMap[client] = nbPartie;
    return {true, ""};
}

const std::pair<bool, QString> LP::nextRound(const usint nbPartie)
{
    QMutexLocker locker(&_mutexLp);
    if(!roomExist(nbPartie))
        return {false, "uncreated room"};
    _rooms.at(nbPartie)->nextRound();
    return {true, ""};
}

void LP::setRunningThreads(std::map<qintptr, ThreadSocket *> *rn)
{
    QMutexLocker locker(&_mutexLp);
    _running_threads = rn;
}

QString LP::playerIcon(QSslSocket *player) const
{
    if(_playersIcon.count(player->objectName()))
        return _playersIcon.at(player->objectName());
    else
        return "";
}

void LP::threadSend(QList<QSslSocket *> destinataires, const QString message) const
{
    for(QSslSocket* soc: destinataires)
    {
        if(_running_threads->count(soc->socketDescriptor()))
            _running_threads->at(soc->socketDescriptor())->send(message);
    }
}

void LP::partyAltered(const std::pair<usint, QStringList> infos) const
{
    if(_roomSelected != infos.first || infos.second.size() < 5 || !roomExist(infos.first))
        return;
    emit roomInfos(infos.second.first(),infos.second.at(1),infos.second.at(2), infos.second.at(3), infos.second.last());
}

void LP::roomSelected(const int nbPartie)
{
    _roomSelected = nbPartie;
    QMutexLocker locker(&_mutexLp);
    if(!roomExist(nbPartie))
        return;
    QStringList infos = _rooms.at(nbPartie)->roomInfos().second;
    emit roomInfos(infos.first(),infos.at(1),infos.at(2), infos.at(3), infos.last());
}

QVariant LP::updatedNetworkInfos() const
{
    return _tcpServer->serverAddress().toString()+":"+QString::number(_tcpServer->serverPort());
}

void LP::writeLogs(const QString &log)
{
    if(!_fileLogs.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "error appending log file : " << _fileLogs.errorString();
    }
    QTextStream out(&_fileLogs);
    out << "["+QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm:ss")+"] "+log+"\n";
    _fileLogs.close();
}
