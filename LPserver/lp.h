#ifndef LP_H
#define LP_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>

#include "server.h"
#include "sqlinteract.h"
#include "party.h"

typedef qint32 packageSize;
class LP : public QObject
{
    Q_OBJECT
public:
    explicit LP(QObject *parent = nullptr);
    virtual ~LP();

    void writeLogs(const QString &log);
    void addClient(QSslSocket *client);
    void removeClient(QSslSocket *client);
    void removePlayer(QSslSocket *client);
    bool userConnected(const QString username, const QString imgData);
    usint createRoom(QSslSocket *client);
    void eraseParty(const usint nPartie);
    usint nbPlayers(const usint nbPartie) const;
    const std::pair<bool, QString> joinRoom(const usint nbPartie, QSslSocket *client);
    const std::pair<bool, QString> nextRound(const usint nbPartie);
    void setRunningThreads(std::map<qintptr,ThreadSocket*> *rn);
    void threadSend(QList<QSslSocket *> destinataires, const QString message) const;
    QString playerIcon(QSslSocket *player) const;

signals:
    void incommingMessage(const QVariant) const;
    void incommingDebug(const QVariant &) const;
    void roomInfos(const QVariant owner, const QVariant nbPlayers, const QVariant playersName,
                   const QVariant dareList, const QVariant truthList) const;
    void newRoom(const QVariant roomId) const;
    void lessRoom(const QVariant roomId) const;

public slots:
    Q_INVOKABLE void roomSelected(const int nbPartie);
    Q_INVOKABLE QVariant updatedNetworkInfos() const;

private:
    void partyAltered(const std::pair<usint, QStringList> infos) const;
    bool roomExist(const usint nPartie) const;

    const QString _primarySeparator     = "⏇∐⏈";
    const QString _secondarySeparator   = "⁑╬₼";
    const QString _ipTcp = "192.168.1.85";//"127.0.0.1";//
    const quint16 _portTcp = 50885;
    Server *_tcpServer;
    SqlInteract *_dataBase;
    QFile _fileLogs;

    std::map<usint, Party*> _rooms;
    std::map<QSslSocket*, usint> _belongingMap;
    std::map<qintptr,ThreadSocket*> *_running_threads;
    std::map<QString, QString> _playersIcon;
    uint _nbDare;
    uint _nbTruth;
    uint _roomSelected = 0;
    QList<QSslSocket*> _connectedSockets;

    QMutex _mutexLp;
    Q_DISABLE_COPY(LP);
};

#endif // LP_H
