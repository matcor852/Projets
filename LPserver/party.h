#ifndef PARTY_H
#define PARTY_H

#include <QObject>
#include <QSslSocket>
#include <QDataStream>

#include "sqlinteract.h"


typedef unsigned short int usint;
typedef unsigned int uint;

class LP;
class Party : public QObject
{
    Q_OBJECT
public:
    explicit Party(QSslSocket *hote, const uint &dares, const uint &truth, SqlInteract *db, LP *root, QObject *parent = nullptr);
    virtual ~Party();

    const std::pair<usint, QStringList> roomInfos();
    void addPlayer(QSslSocket *newPlayer);
    void delPlayer(QSslSocket *oldPlayer);
    void nextRound();
    QString players() const;
    QString playersIcons() const;
    usint nbPlayers() const;
    QString gageList(const QString &table) const;
    QString owner() const;
    void imposeConstraint(const QString &name, const QString &constraint);

signals:
    void deletedRoom(const usint) const;
    void altered(const std::pair<usint, QStringList>) const;

private:
    void send(const QList<QSslSocket*> destinataires, const QString message);
    void send(QSslSocket* destinataire, const QString message);
    usint randPlayer(const QString &sender = "") const;
    QString pickNameByGender(const QString &separ, const QString &sender) const;
    bool mixite() const;

    QList<QSslSocket *> _playersSoc;
    QSslSocket *_owner;
    SqlInteract *_dataBase;
    LP *_root;

    QMutex _mutexRoom;
    Q_DISABLE_COPY(Party)

    QList<usint> _dareAppeared;
    QList<usint> _truthAppeared;
    const uint _nbDare;
    const uint _nbTruth;
    bool _mixite;
    usint _lastIndexPlayerPicked;
    const QList<QString> _tables = {"dare","truth"};
    const QString _primarySeparator     = "⏇∐⏈";
    const QString _secondarySeparator   = "⁑╬₼";
    const QString _playersSeparator = "<br/>";
    const QString _playerInfosSeparator = "⁑∐₼";

    enum _commands
    {
        roomCreation    = 2,
        roomJoin        = 3,
        playersList     = 13,
        partyNextRound  = 15,
        inheritParty    = 16,
        errorOccured    = 100
    };
};

#endif // PARTY_H
