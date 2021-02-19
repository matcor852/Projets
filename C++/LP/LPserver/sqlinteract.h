#ifndef SQLINTERACT_H
#define SQLINTERACT_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringBuilder>
#include <QMutex>
#include <QMutexLocker>

#include <QDebug>

typedef unsigned int uint;

class SqlInteract : public QObject
{
    Q_OBJECT

public:
    explicit SqlInteract(QObject *parent, const QString &path);

    std::pair<bool,QString> CreateUser(const QString &nom, const QString &prenom, const QString &age,
                                       const QString &pseudo, const QString &genre, const QString &password, const QString &hostname);
    std::pair<bool,QString> Identify(const QString &user, const QString &pass);
    uint nbGages(const QString &table);
    QString retrieveGender(const QString &name);
    std::pair<uint, QString> retrieveGage(const QString &table, const QString &gender, const QString &alreadyAppeared);

signals:
    void debugInfos(const QString &) const;

private:
    bool testStrContent(const QString &string) const;

    QSqlDatabase _db;
    QSqlQuery _query;
    uint _nbDare;
    uint _nbTruth;
    QVector<QString> forbiddenKeywords;

    QMutex _mutexDb;
    Q_DISABLE_COPY(SqlInteract);
};

#endif // SQLINTERACT_H
