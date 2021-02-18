#include "sqlinteract.h"

SqlInteract::SqlInteract(QObject *parent, const QString &path)
    :QObject(parent), _mutexDb()
{
    const QString DRIVER("QSQLITE");
    if(!QSqlDatabase::isDriverAvailable(DRIVER))
    {
        qWarning() << "Driver "%DRIVER%" not available";
    }
    _db = QSqlDatabase::addDatabase(DRIVER);
    _db.setDatabaseName(path);
    if(!_db.open())
    {
        qWarning() << "ERROR: " << _db.lastError();
    }
    _query = QSqlQuery(_db);

    forbiddenKeywords.append("ABORT");
    forbiddenKeywords.append("ACTION");
    //forbiddenKeywords.append("ADD");
    forbiddenKeywords.append("AFTER");
    //forbiddenKeywords.append("ALL");
    forbiddenKeywords.append("ALTER");
    forbiddenKeywords.append("ALWAYS");
    forbiddenKeywords.append("ANALYZE");
    //forbiddenKeywords[9] = "AND";
    //forbiddenKeywords[10] = "AS";
    //forbiddenKeywords[11] = "ASC";
    forbiddenKeywords.append("ATTACH");
    forbiddenKeywords.append("AUTOINCREMENT");
    forbiddenKeywords.append("BEFORE");
    forbiddenKeywords.append("BEGIN");
    forbiddenKeywords.append("BETWEEN");
    //forbiddenKeywords[17] = "BY";
    forbiddenKeywords.append("CASCADE");
    forbiddenKeywords.append("CASE");
    forbiddenKeywords.append("CAST");
    forbiddenKeywords.append("CHECK");
    forbiddenKeywords.append("COLLATE");
    forbiddenKeywords.append("COLUMN");
    forbiddenKeywords.append("COMMIT");
    forbiddenKeywords.append("CONFLICT");
    forbiddenKeywords.append("CONSTRAINT");
    forbiddenKeywords.append("CREATE");
    forbiddenKeywords.append("CROSS");
    forbiddenKeywords.append("CURRENT");
    forbiddenKeywords.append("CURRENT_DATE");
    forbiddenKeywords.append("CURRENT_TIME");
    forbiddenKeywords.append("CURRENT_TIMESTAMP");
    forbiddenKeywords.append("DATABASE");
    forbiddenKeywords.append("DEFAULT");
    forbiddenKeywords.append("DEFERRABLE");
    forbiddenKeywords.append("DEFERRED");
    forbiddenKeywords.append("DELETE");
    //forbiddenKeywords[38] = "DESC";
    forbiddenKeywords.append("DETACH");
    forbiddenKeywords.append("DISTINCT");
    //forbiddenKeywords[41] = "DO";
    forbiddenKeywords.append("DROP");
    forbiddenKeywords.append("EACH");
    forbiddenKeywords.append("ELSE");
    //forbiddenKeywords[45] = "END";
    forbiddenKeywords.append("ESCAPE");
    forbiddenKeywords.append("EXCEPT");
    forbiddenKeywords.append("EXCLUDE");
    forbiddenKeywords.append("EXCLUSIVE");
    forbiddenKeywords.append("EXISTS");
    forbiddenKeywords.append("EXPLAIN");
    forbiddenKeywords.append("FAIL");
    forbiddenKeywords.append("FILTER");
    forbiddenKeywords.append("FIRST");
    forbiddenKeywords.append("FOLLOWING");
    //forbiddenKeywords[56] = "FOR";
    forbiddenKeywords.append("FOREIGN");
    forbiddenKeywords.append("FROM");
    forbiddenKeywords.append("FULL");
    forbiddenKeywords.append("GENERATED");
    forbiddenKeywords.append("GLOB");
    forbiddenKeywords.append("GROUP");
    forbiddenKeywords.append("GROUPS");
    forbiddenKeywords.append("HAVING");
    //forbiddenKeywords[65] = "IF";
    forbiddenKeywords.append("IGNORE");
    forbiddenKeywords.append("IMMEDIATE");
    //forbiddenKeywords[68] = "IN";
    forbiddenKeywords.append("INDEX");
    forbiddenKeywords.append("INDEXED");
    forbiddenKeywords.append("INITIALLY");
    forbiddenKeywords.append("INNER");
    forbiddenKeywords.append("INSERT");
    forbiddenKeywords.append("INSTEAD");
    forbiddenKeywords.append("INTERSECT");
    forbiddenKeywords.append("INTO");
    //forbiddenKeywords[77] = "IS";
    forbiddenKeywords.append("ISNULL");
    forbiddenKeywords.append("JOIN");
    forbiddenKeywords.append("KEY");
    forbiddenKeywords.append("LAST");
    forbiddenKeywords.append("LEFT");
    forbiddenKeywords.append("LIKE");
    forbiddenKeywords.append("LIMIT");
    forbiddenKeywords.append("MATCH");
    forbiddenKeywords.append("NATURAL");
    //forbiddenKeywords[87] = "NO";
    //forbiddenKeywords.append("NOT");
    forbiddenKeywords.append("NOTHING");
    forbiddenKeywords.append("NOTNULL");
    forbiddenKeywords.append("NULL");
    forbiddenKeywords.append("NULLS");
    //forbiddenKeywords[93] = "OF";
    forbiddenKeywords.append("OFFSET");
    //forbiddenKeywords[95] = "ON";
    //forbiddenKeywords[96] = "OR";
    forbiddenKeywords.append("ORDER");
    forbiddenKeywords.append("OTHERS");
    forbiddenKeywords.append("OUTER");
    forbiddenKeywords.append("OVER");
    forbiddenKeywords.append("PARTITION");
    forbiddenKeywords.append("PLAN");
    forbiddenKeywords.append("PRAGMA");
    forbiddenKeywords.append("PRECEDING");
    forbiddenKeywords.append("PRIMARY");
    forbiddenKeywords.append("QUERY");
    forbiddenKeywords.append("RAISE");
    forbiddenKeywords.append("RANGE");
    forbiddenKeywords.append("RECURSIVE");
    forbiddenKeywords.append("REFERENCES");
    forbiddenKeywords.append("REGEXP");
    forbiddenKeywords.append("REINDEX");
    forbiddenKeywords.append("RELEASE");
    forbiddenKeywords.append("RENAME");
    forbiddenKeywords.append("REPLACE");
    forbiddenKeywords.append("RESTRICT");
    forbiddenKeywords.append("RIGHT");
    forbiddenKeywords.append("ROLLBACK");
    forbiddenKeywords.append("ROW");
    forbiddenKeywords.append("ROWS");
    forbiddenKeywords.append("SAVEPOINT");
    forbiddenKeywords.append("SELECT");
    //forbiddenKeywords.append("SET");
    forbiddenKeywords.append("TABLE");
    forbiddenKeywords.append("TEMP");
    forbiddenKeywords.append("TEMPORARY");
    forbiddenKeywords.append("THEN");
    forbiddenKeywords.append("TIES");
    //forbiddenKeywords[130] = "TO";
    forbiddenKeywords.append("TRANSACTION");
    forbiddenKeywords.append("TRIGGER");
    forbiddenKeywords.append("UNBOUNDED");
    forbiddenKeywords.append("UNION");
    forbiddenKeywords.append("UNIQUE");
    forbiddenKeywords.append("UPDATE");
    forbiddenKeywords.append("USING");
    forbiddenKeywords.append("VACUUM");
    forbiddenKeywords.append("VALUES");
    forbiddenKeywords.append("VIEW");
    forbiddenKeywords.append("VIRTUAL");
    forbiddenKeywords.append("WHERE");
    forbiddenKeywords.append("WINDOW");
    forbiddenKeywords.append("WITH");
    forbiddenKeywords.append("WITHOUT");
}

std::pair<bool, QString> SqlInteract::CreateUser(const QString &nom, const QString &prenom, const QString &age, const QString &pseudo, const QString &genre, const QString &password, const QString &hostname)
{
    if(pseudo.length()<3 || pseudo.length()>15 || genre.length() < 1)
        return {false,"Erreur : vérifiez la longueur de vos entrées"};
    if(testStrContent(nom) || testStrContent(prenom) || testStrContent(pseudo) | testStrContent(password))
        return {false,"Erreur : valeur(s) interdite(s)"};
    QMutexLocker locker(&_mutexDb);
    if(!_db.open())
        return {false,_db.lastError().text()};
    if(!_query.exec("INSERT INTO Users(nOm,prEnOm,AgE,UserName,pASSwORD,Genre,HostName) VALUES('"%nom%"','"%prenom%"','"%age%"','"%pseudo%"','"%password%"','"%genre[0].toLower()%"','"%hostname%"')"))
        return {false,_query.lastError().text()};
    _db.close();
    return {true,pseudo};
}

std::pair<bool, QString> SqlInteract::Identify(const QString &user, const QString &pass)
{
    if(user.length()<3 || user.length()>25 || pass.length()>32 || pass.length()<6)
        return {false,"Erreur : vérifiez la longueur de vos entrées"};
    if(testStrContent(user) || testStrContent(pass))
        return {false,"Erreur : valeur(s) interdites"};
    QMutexLocker locker(&_mutexDb);
    if(!_db.open())
    {
        return {false,_db.lastError().text()};
    }
    if(!_query.exec("SELECT EXISTS (SELECT 1 FROM Users WHERE UserName='"%user%"' AND pASSwORD='"%pass%"')"))
    {
        return {false,_query.lastError().text()};
    }
    if(!_query.next())
    {
        return {false,_query.lastError().text()};
    }
    return {_query.value(0).toBool(),user};
}

void SqlInteract::addConfigInfos(const QString &user, const QString &HostName, const QString &ip)
{
    if(testStrContent(user) || testStrContent(HostName) || testStrContent(ip))
    {
        return;
    }
    QMutexLocker locker(&_mutexDb);
    if(!_db.open())
    {
        qDebug() << _db.lastError().text();
        emit debugInfos(_db.lastError().text());
        return;
    }
    if(!_query.exec("UPDATE Users SET HostName= HostName || '"%HostName%";' WHERE UserName = '"%user%"' AND HostName NOT LIKE '%"%HostName%";%'"))
    {
        qDebug() << _query.lastError().text();
        emit debugInfos(_query.lastError().text());
    }
    if(!_query.exec("UPDATE Users SET Ip= Ip || '"%ip%";' WHERE UserName = '"%user%"' AND Ip NOT LIKE '%"%ip%";%'"))
    {
        qDebug() << _query.lastError().text();
        emit debugInfos(_query.lastError().text());
    }
    _db.close();
}

uint SqlInteract::nbGages(const QString &table)
{
    if(testStrContent(table))
    {
        return 0;
    }
    QMutexLocker locker(&_mutexDb);
    if(!_db.open())
    {
        qDebug() << _db.lastError().text() << " for " << table;
        emit debugInfos(_db.lastError().text() + " for " + table);
        return 0;
    }
    if(!_query.exec("SELECT COUNT(ROWID) FROM "%table))
    {
        qDebug() << _query.lastError().text() << " for " << table;
        emit debugInfos(_query.lastError().text() + " for " + table);
        return 0;
    }
    _query.last();
    qDebug() << _query.value(0).toInt() << " action for "%table;
    emit debugInfos(QString::number(_query.value(0).toInt()) + " action for "%table);
    return _query.value(0).toInt();
}

QString SqlInteract::retrieveGender(const QString &name)
{
    if(testStrContent(name))
    {
        return 0;
    }
    QMutexLocker locker(&_mutexDb);
    if(!_db.open())
    {
        qDebug() << _db.lastError().text();
        emit debugInfos(_db.lastError().text());
        return 0;
    }
    if(!_query.exec("SELECT Genre FROM Users WHERE UserName IS '"%name%"'"))
    {
        qDebug() << _query.lastError().text();
        emit debugInfos(_query.lastError().text());
        return 0;
    }
    _query.last();
    return _query.value(0).toString();
}

std::pair<uint, QString> SqlInteract::retrieveGage(const QString &table, const QString &gender, const QString &alreadyAppeared)
{
    if(testStrContent(alreadyAppeared))
    {
        return {};
    }
    QMutexLocker locker(&_mutexDb);
    if(!_db.open())
    {
        qDebug() << _db.lastError().text();
        emit debugInfos(_db.lastError().text());
        return {};
    }
    if(!_query.exec("SELECT ROWID,action from "%table%" WHERE gender IS NOT '"%QString(gender)%"' AND ROWID NOT IN ("%alreadyAppeared%") ORDER BY random() LIMIT 1"))
    {
        qDebug() << _query.lastError().text();
        emit debugInfos(_query.lastError().text());
        return {};
    }
    std::pair<unsigned int, QString> res;
    if(!_query.next())
    {
        //trying recursive
        res = retrieveGage(table,"z",alreadyAppeared);
    }
    else
    {
        res = {_query.value(0).toUInt(), _query.value(1).toString()};
    }
    return res;
}

bool SqlInteract::testStrContent(const QString &string) const
{
    QVectorIterator<QString> itKeywords(forbiddenKeywords);
    const QString strL = string.toLower();
    const QString strU = string.toUpper();
    while(itKeywords.hasNext())
    {
        if(strL.contains(itKeywords.next().toLower()) || strU.contains(itKeywords.next().toUpper()))
        {
            emit debugInfos("! ! Possible SQL injection detected in request ! !");
            return true;
        }
    }
    return false;
}
