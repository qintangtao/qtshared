#include "SqlHelper.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QVariant>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>

#include "BRMYunJson.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(logSqlHelper, "log.sqlite.helper")

template <typename T>
SqlHelper<T>::SqlHelper(QSqlDatabase *db)
    : m_db(db)
{
    createTable();
}

template <typename T>
SqlHelper<T>::~SqlHelper()
{
}

template <typename T>
bool SqlHelper<T>::insert(T *obj)
{
    QStringList sqlProperty;
    QStringList sqlValue;

    QMetaObject meta = T::staticMetaObject;
    int count = meta.propertyCount();
    for (int i = 0; i < count; i++) {
        QMetaProperty p  = meta.property(i);

        if (!p.isReadable())
            continue;

        QString name = p.name();
        if (!name.startsWith(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR)))
            continue;

        name.remove(0, QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR).length());

        QVariant v = p.read(obj);

        switch (p.userType()) {
        case QVariant::Bool:
            sqlProperty.append(name);
            sqlValue.append(QString("%1").arg(v.toBool() ? 1 : 0));
            break;
        case QVariant::Int:
            //sqlProperty.append(QString("%1").arg(v.toInt()));
            break;
        case QVariant::UInt:
            sqlProperty.append(name);
            sqlValue.append(QString("%1").arg(v.toUInt()));
            break;
        case QVariant::LongLong:
            sqlProperty.append(name);
            sqlValue.append(QString("%1").arg(v.toLongLong()));
            break;
        case QVariant::ULongLong:
            sqlProperty.append(name);
            sqlValue.append(QString("%1").arg(v.toULongLong()));
            break;
        case QVariant::Double:
            sqlProperty.append(name);
            sqlValue.append(QString("%1").arg(v.toDouble()));
            break;
        case QVariant::String:
            sqlProperty.append(name);
            sqlValue.append(QString("'%1'").arg(v.toString()));
            break;
        default:
            break;
        }
    }

    QStringList sql;
    sql.append("INSERT");
    sql.append("INTO");
    sql.append(tableName());
    sql.append("(");
    sql.append(sqlProperty.join(','));
    sql.append(")");
    sql.append("VALUES");
    sql.append("(");
    sql.append(sqlValue.join(','));
    sql.append(");");

    return execSql(sql.join(' '));
}

template <typename T>
bool SqlHelper<T>::update(T *obj, const QVariantMap& where)
{
    QStringList sql;
    sql.append("UPDATE");
    sql.append(tableName());
    sql.append("SET");

    sql.append("WHERE");
    sql.append(whereSql(where));
    sql.append(";");

#if 0
    QSqlQuery query( m_dbconn );
    query.exec( QString("UPDATE BSDisplays "
                        "SET bsSegmentation = '%1'"
                        "WHERE bsId = '%2' AND bsGroupDisplayGUID "
                        "IN (SELECT bsGUID FROM BSGroupDisplays WHERE bsRoomGUID "
                        "IN (SELECT bsGUID FROM BSRooms WHERE bsId = '%3' AND bsMacAddr = '%4'))")
                .arg( segment )
                .arg( Displayid )
                .arg( roomid )
                .arg( bsMacAddr ));
#endif

    return execSql(sql.join(' '));
}

template <typename T>
bool SqlHelper<T>::update(T *obj)
{
    QVariantMap where;

    //自动查找主键，并填入
    //where.insert("id", v);

    return update(where);
}

template <typename T>
bool SqlHelper<T>::remove(const QVariantMap& where)
{
    QStringList sql;
    sql.append("DELETE");
    sql.append("FROM");
    sql.append(tableName());
    sql.append("WHERE");
    sql.append(whereSql(where));
    sql.append(";");

    return execSql(sql.join(' '));
}

template <typename T>
bool SqlHelper<T>::remove(const QVariant& v)
{
    QVariantMap where;

    //自动查找主键，并填入
    where.insert("id", v);

    return remove(where);
}

template <typename T>
bool SqlHelper<T>::queryAll(QList<T *> &list, const QVariantMap& where)
{
    bool r = false;

    QStringList sql;
    sql.append("SELECT");
    sql.append("*");
    sql.append("FROM");
    sql.append(tableName());

    if (!where.isEmpty())
    {
        sql.append("WHERE");
        sql.append(whereSql(where));
    }

    sql.append(";");

    m_db->transaction();

    QSqlQuery query( *m_db );

    r  = query.exec(sql.join(' '));
    if (r)
    {
        QStringList filedNames;
        QList<QMetaProperty> propertys;

        QSqlRecord r = query.record();
        QMetaObject meta = T::staticMetaObject;

        for (int i = 0; i < r.count(); i++) {

            QString fieldName = r.fieldName(i);

            int id = meta.indexOfProperty(QString("%1%2")
                                          .arg(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR))
                                          .arg(fieldName)
                                          .toLocal8Bit().constData());

            QSqlField sqlField = r.field(i);

            qCDebug(logSqlHelper) << fieldName << id << sqlField.isAutoValue();

            if (-1 == id)
                continue;

            filedNames.append(fieldName);
            propertys.append(meta.property(id));
        }

        while ( query.next() ) {
            T *t = T::newObject();
            for (int i = 0; i < filedNames.count(); i++) {
                const QVariant &v = query.value(filedNames.at(i));
                const QMetaProperty &p = propertys.at(i);
                p.write(t, v);
            }
            list.append(t);
        }
    }

    m_db->commit();

    return r;
}

template <typename T>
bool SqlHelper<T>::createTable()
{
    QStringList sqlProperty;

    QMetaObject meta = T::staticMetaObject;
    int count = meta.propertyCount();
    for (int i = 0; i < count; i++) {
        QMetaProperty p  = meta.property(i);

        if (!p.isReadable())
            continue;

        QString name = p.name();
        if (!name.startsWith(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR))) {
            continue;
        }

        name.remove(0, QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR).length());

        switch (p.userType()) {
        case QVariant::Bool:
            sqlProperty.append(QString("%1 INTEGER").arg(name));
            break;
        case QVariant::Int:
             sqlProperty.append(QString("%1 INTEGER PRIMARY KEY AUTOINCREMENT").arg(name));
            break;
        case QVariant::UInt:
        case QVariant::LongLong:
        case QVariant::ULongLong:
            sqlProperty.append(QString("%1 INTEGER").arg(name));
            break;
        case QVariant::Double:
            sqlProperty.append(QString("%1 REAL").arg(name));
            break;
        case QVariant::String:
            sqlProperty.append(QString("%1 TEXT").arg(name));
            break;
        default:
            break;
        }
    }

    QStringList sql;
    sql.append("CREATE");
    sql.append("TABLE");
    sql.append("IF");
    sql.append("NOT");
    sql.append("EXISTS");
    sql.append(tableName());
    sql.append("(");
    sql.append(sqlProperty.join(','));
    sql.append(");");

    return execSql(sql.join(' '));
}

template <typename T>
bool SqlHelper<T>::execSql(const QString &sql, bool transaction)
{
    bool r = false;

    qCDebug(logSqlHelper) << sql << transaction;

    if (!m_db) {
        qCWarning(logSqlHelper) << "db is null";
        return r;
    }

    QSqlQuery query( *m_db );

    if (transaction)
        m_db->transaction();
    r = query.exec(sql);
    if (transaction)
        m_db->commit();

    if (!r)
        qCCritical(logSqlHelper) << sql << "\r\n" << query.lastError().text();

    return r;
}

template <typename T>
QString SqlHelper<T>::tableName()
{
    QMetaObject meta = T::staticMetaObject;
    return meta.className();
}

template <typename T>
QString SqlHelper<T>::whereSql(const QVariantMap& where)
{
    QStringList sql;

    QMetaObject meta = T::staticMetaObject;

    foreach(QString key, where.keys())
    {
        int id = meta.indexOfProperty(QString("%1%2")
                                      .arg(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR))
                                      .arg(key)
                                      .toLocal8Bit().constData());
        if (-1 == id) {
            qCWarning(logSqlHelper) << meta.className() <<  key << "not found";
            continue;
        }

        qCDebug(logSqlHelper) << key << id;

        const QVariant &v = where.value(key);

        QMetaProperty p  = meta.property(id);

        switch (p.userType()) {
        case QVariant::Bool:
            sql.append(QString("%1=%2").arg(key).arg(v.toBool() ? 1 : 0));
            break;
        case QVariant::Int:
            sql.append(QString("%1=%2").arg(key).arg(v.toInt()));
            break;
        case QVariant::UInt:
            sql.append(QString("%1=%2").arg(key).arg(v.toUInt()));
            break;
        case QVariant::LongLong:
            sql.append(QString("%1=%2").arg(key).arg(v.toLongLong()));
            break;
        case QVariant::ULongLong:
            sql.append(QString("%1=%2").arg(key).arg(v.toULongLong()));
            break;
        case QVariant::Double:
            sql.append(QString("%1=%2").arg(key).arg(v.toDouble()));
            break;
        case QVariant::String:
            sql.append(QString("%1=%2").arg(key).arg(v.toString()));
            break;
        default:
            break;
        }
    }

    return sql.join(" AND ");
}

void test_SqlHelper()
{
    BRMYunJson json2;
    json2.set_cmd_top("1");
    json2.set_cmd_bottom("2");
    json2.set_cmd_left("3");
    json2.set_cmd_right("4");
    json2.set_test_int(123);
    json2.set_test_bool(true);
    json2.set_test_double(33.4);

#define CONNECTNAME "BSDB"
#define USER        "sa"
#define PASSWORD    "Br123456"

    QSqlDatabase    m_dbconn;
    if (QSqlDatabase::contains( CONNECTNAME ))
        m_dbconn = QSqlDatabase::database( CONNECTNAME );    //添加数据库驱动
    else
        m_dbconn = QSqlDatabase::addDatabase("QSQLITE", CONNECTNAME);    //添加数据库驱动

    if ( m_dbconn.isOpenError() )
        return;

    m_dbconn.setDatabaseName("test.db");  //在工程目录新建一个 *.db 的文件
    m_dbconn.setUserName( USER );
    m_dbconn.setPassword( PASSWORD );

    if( !m_dbconn.open() )
        return;

    SqlHelper<BRMYunJson> helper(&m_dbconn);
    helper.insert(&json2);

   // json2.set_test_int(3);
    json2.set_test_double(33.5);
    json2.set_cmd_top("2");

    QVariantMap map;
    map.insert("test_int", 9);
    helper.remove(map);

    QVariantMap map2;
    map2.insert("cmd_right", "4");
    QList<BRMYunJson *> list;
    helper.queryAll(list, map2);

    foreach(BRMYunJson *pJson, list)
    {
        qCDebug(logSqlHelper) << pJson->toString();
    }

    m_dbconn.close();

}
