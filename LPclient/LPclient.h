#ifndef LPCLIENT_H
#define LPCLIENT_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkProxy>
#include <QFile>
#include <QHostInfo>
#include <math.h>
#include <QDataStream>
#include <QStringBuilder>
#include <QCryptographicHash>
#include <QTimer>
#include <QStringList>

typedef qint32 packageSize;
class LPclient : public QObject
{
    Q_OBJECT

public:
    explicit LPclient(QObject *parent = nullptr);
    ~LPclient();

signals:
    void appendInfos(const QVariant &infos) const;
    void swiper(const QVariant &pageNb) const;
    void authented() const;
    void integrated(const QVariant &nbParty) const;
    void refreshPlayers(const QVariant imgData, const QVariant &playersList, const QVariant &removeOrder) const;
    void newAction(const QVariant &imgData, const QVariant &playerName, const QVariant &action) const;
    void inheritedParty(const QVariant &owned) const;

public slots:
    Q_INVOKABLE void login(const QString &username, const QString &password, const QVariant iconPath);
    Q_INVOKABLE void askAccountCreation(const QString &nom, const QString &prenom, const unsigned int &age, const QString &pseudo, const QString &genre, const QString &password) const;
    Q_INVOKABLE void askCreate() const;
    Q_INVOKABLE void askJoin(const unsigned int &partyNb) const;
    Q_INVOKABLE void askNext() const;
    Q_INVOKABLE void askQuit() const;

private slots:
    void dataReceived();
    void connecte() const;
    void deconnecte() const;
    void erreurSocket(const QAbstractSocket::SocketError &erreur) const;
    void sslErrors(const QList<QSslError> &errors) const;

private:
    void allocateOrders(const QString &order);
    void send(const QString msg) const;
    const QString passwordHash(const QString &password) const;

    const QString _primarySeparator     = "⏇∐⏈";
    const QString _secondarySeparator   = "⁑╬₼";
    const QString _playersSeparator = "<br/>";
    const QString _playerInfosSeparator = "⁑∐₼";
    QString _username = "";
    QString _password = "";
    const QString _ip = "matcor852.dynv6.net";//"127.0.0.1";//
    const quint16 _port = 50885;
    packageSize _tailleMessage;
    unsigned int _nbParty;
    QSslSocket *_socket;
    QTimer *_connexionTimer;
    std::map<QString, QString> _playersIcon;

    enum _commands
    {
        accountCreation = 0,
        authentication  = 1,
        roomCreation    = 2,
        roomJoin        = 3,
        playersList     = 13,
        partyNextRound  = 15,
        inheritParty    = 16,
        error           = 100
    };
};

#endif // LPCLIENT_H
