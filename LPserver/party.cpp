#include "party.h"
#include "lp.h"

Party::Party(QSslSocket *hote, const uint &dares, const uint &truth, SqlInteract *db, LP *root, QObject *parent)
    : QObject(parent), _owner(hote), _dataBase(db), _root(root), _mutexRoom(),
      _nbDare(dares), _nbTruth(truth), _lastIndexPlayerPicked(-1)
{
    QMutexLocker locker(&_mutexRoom);
    _playersSoc.append(hote);
}

Party::~Party()
{
    QMutexLocker locker(&_mutexRoom);
    _root->writeLogs("in room n°"+this->objectName()+" deletion");
    _playersSoc.clear();
    emit deletedRoom(this->objectName().toUInt());
}

const std::pair<usint, QStringList> Party::roomInfos()
{
    QMutexLocker locker(&_mutexRoom);
    return {this->objectName().toUInt(), {_owner->objectName(), QString::number(nbPlayers()), players(), gageList("dare"), gageList("truth")}};
}

void Party::addPlayer(QSslSocket *newPlayer)
{
    QMutexLocker locker(&_mutexRoom);
    _playersSoc.append(newPlayer);
    send(newPlayer, QString::number(_commands::roomJoin)%_primarySeparator%this->objectName());
    _mixite = mixite();
    send(_playersSoc, QString::number(_commands::playersList)%_primarySeparator%playersIcons());
    emit altered({this->objectName().toUInt(), {_owner->objectName(), QString::number(nbPlayers()), players(), gageList("dare"), gageList("truth")}});
}

void Party::send(QSslSocket *destinataire, const QString message)
{
    QList<QSslSocket *> destinataires;
    destinataires.append(destinataire);
    _root->threadSend(destinataires, message);
}

void Party::send(const QList<QSslSocket *> destinataires, const QString message)
{
    _root->threadSend(destinataires, message);
}

void Party::delPlayer(QSslSocket *oldPlayer)
{
    QMutexLocker locker(&_mutexRoom);
    _playersSoc.removeOne(oldPlayer);
    if(oldPlayer == _owner && _playersSoc.length())
    {
        _owner = _playersSoc.at(_playersSoc.length()-1);
        send(_owner, QString::number(_commands::inheritParty)%_primarySeparator%"inherited");
    }
    if(_playersSoc.isEmpty())
    {
        this->deleteLater();
        return;
    }
    _mixite = mixite();
    send(_playersSoc, QString::number(_commands::playersList)%_primarySeparator%playersIcons());
    emit altered({this->objectName().toUInt(), {_owner->objectName(), QString::number(nbPlayers()), players(), gageList("dare"), gageList("truth")}});
}

void Party::nextRound()
{
    QMutexLocker locker(&_mutexRoom);
    if(_playersSoc.size() <= 1)
    {
        send(_playersSoc, QString::number(_commands::errorOccured)%_primarySeparator%"Moins de 2 joueurs dans la partie");
        return;
    }
    const QString table = _tables.at(rand()%2);
    const usint playerPicked = randPlayer();
    _lastIndexPlayerPicked = playerPicked;
    const QString gender = _dataBase->retrieveGender(_playersSoc.at(playerPicked)->objectName());
    const std::pair<uint, QString> reply = _dataBase->retrieveGage(table, gender, gageList(table));
    if(!reply.first || reply.second.isEmpty())
    {
        _root->writeLogs("\t! Unable to reach a reply in room "+this->objectName()+" !");
        return;
    }
    ((table=="dare")?_dareAppeared:_truthAppeared).append(reply.first);
    QString gage = reply.second;

    if(gage.contains("%1"))
        gage = gage.arg(pickNameByGender("%1",_playersSoc.at(playerPicked)->objectName()));
    if(gage.contains("%2"))
        gage = gage.arg(pickNameByGender("%2",_playersSoc.at(playerPicked)->objectName()));
    if(gage.contains("%3"))
        gage = gage.arg(_playersSoc.at(randPlayer())->objectName());

    if(unsigned(_dareAppeared.size()) == _nbDare)
    {
        _dareAppeared.clear();
    }
    if(unsigned(_truthAppeared.size()) == _nbTruth)
    {
        _truthAppeared.clear();
    }
    send(_playersSoc, QString::number(_commands::partyNextRound)%_primarySeparator%_playersSoc.at(playerPicked)->objectName()%_secondarySeparator%gage);
    emit altered({this->objectName().toUInt(), {_owner->objectName(), QString::number(nbPlayers()), players(), gageList("dare"), gageList("truth")}});
}

QString Party::players() const
{
    QString players;
    for(QSslSocket* soc : _playersSoc)
    {
        players.append(soc->objectName()%_playersSeparator);
    }
    players.chop(_playersSeparator.length());
    return players;
}

QString Party::playersIcons() const
{
    QString players;
    for(QSslSocket* soc : _playersSoc)
    {
        players.append(soc->objectName()%_playerInfosSeparator%_root->playerIcon(soc)%_playersSeparator);
    }
    players.chop(_playersSeparator.length());
    return players;
}

usint Party::nbPlayers() const
{
    return _playersSoc.size();
}

QString Party::gageList(const QString &table) const
{
    QString alreadyAppeared;
    for(const usint &nb : (table=="dare")?_dareAppeared:_truthAppeared)
    {
        alreadyAppeared.append(QString::number(nb)%",");
    }
    alreadyAppeared.chop(1);
    return alreadyAppeared;
}

QString Party::owner() const
{
    return _owner->objectName();
}

usint Party::randPlayer(const QString &sender) const
{
    usint indexPlayerPicked;
    do
    {
        indexPlayerPicked = rand()%_playersSoc.size();
    } while (indexPlayerPicked == _lastIndexPlayerPicked || _playersSoc.at(indexPlayerPicked)->objectName() == sender);
    return indexPlayerPicked;
}

QString Party::pickNameByGender(const QString &separ, const QString &sender) const
{
    QString gender = (separ=="%1")?"f":"h";
    usint player;
    QString genre;
    do
    {
        player = randPlayer();
        genre = _dataBase->retrieveGender(_playersSoc.at(player)->objectName());
    }while(genre != gender || (_mixite) ? _playersSoc.at(player)->objectName() == sender:false);
    return _playersSoc.at(player)->objectName();
}

bool Party::mixite() const
{
    usint h = 0, f = 0;
    for(QTcpSocket* socket : _playersSoc)
    {
        const QString genre = _dataBase->retrieveGender(socket->objectName());
        if(genre == "f")
            f++;
        else
            h++;
    }
    if(h >= 2  && f >= 2)
        return true;
    else
        return false;
}
