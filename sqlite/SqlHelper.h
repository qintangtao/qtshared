#ifndef SqlHelper_H
#define SqlHelper_H

#include <QMetaObject>
#include <QMetaProperty>
#include <QSqlDatabase>
#include <QStringList>
#include <QList>

#define JSON_CONVERT_STR_(x)      #x
#define JSON_CONVERT_STR(x)       JSON_CONVERT_STR_(x)

#define JSON_CONVERT_CONS_(x, y) x##y
#define JSON_CONVERT_CONS(x, y)  JSON_CONVERT_CONS_(x, y)

#define JSON_CONVERT_PROPERTY_NAME_PREFIX                  SqlHelper_
#define JSON_CONVERT_PROPERTY_NAME_PREFIX_STR          JSON_CONVERT_STR(JSON_CONVERT_PROPERTY_NAME_PREFIX)

#define JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX                  SqlHelperCreate_
#define JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX_STR          JSON_CONVERT_STR(JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX)



#define JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX                  SqlHelperCreate_
#define JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX_STR          JSON_CONVERT_STR(JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX)

#define JSON_CONVERT_PROPERTY(type, name, ...) \
    Q_PROPERTY(type JSON_CONVERT_CONS(JSON_CONVERT_PROPERTY_NAME_PREFIX, name) __VA_ARGS__)

#define JSON_CONVERT_CREATE_PROPERTY(type, name, ...) \
    Q_PROPERTY(type JSON_CONVERT_CONS(JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX, name) __VA_ARGS__)

#define JSON_CONVERT_SETMETHOD(CLASSNAME, PROPERTYNAME, SETNAME) \
    inline void SETNAME( CLASSNAME v) {PROPERTYNAME = v;}

#define JSON_CONVERT_GETMETHOD(CLASSNAME, PROPERTYNAME, GETNAME) \
    inline CLASSNAME  GETNAME() const { return PROPERTYNAME; }

#define JSON_CONVERT_METHOD(CLASSNAME, PROPERTYNAME, SETNAME, GETNAME) \
    JSON_CONVERT_SETMETHOD(CLASSNAME, PROPERTYNAME, SETNAME) \
    JSON_CONVERT_GETMETHOD(CLASSNAME, PROPERTYNAME, GETNAME)

#define JSON_CONVERT_CREATE_METHOD(CLASSNAME,  GETNAME) \
    inline CLASSNAME  *GETNAME() const { return new CLASSNAME() ; }


#define Q_NEW_OBJECT(T) static T *newObject() { return new T(NULL); }


template <typename T>
class SqlHelper
{
public:
    explicit SqlHelper(QSqlDatabase *db);
    ~SqlHelper();

    bool insert(T *obj);

    bool update(T *obj, const QVariantMap& where);

    bool update(T *obj);

    bool remove(const QVariantMap& where);

    bool remove(const QVariant& v);

    bool queryOne(T &obj, const QVariantMap& where = QVariantMap());

    bool queryOne(T &obj);

    bool queryAll(QList<T *> &list, const QVariantMap& where = QVariantMap());

    bool execSql(const QString &sql, bool transaction = true);

private:
    void readPropertys();

    bool createTable();

    QString originalProperty(const QString &name);

    QString tableName();

    QString whereSql(const QVariantMap& where);

    QString setSql(T *obj, const QStringList& ignore);



private:
    QSqlDatabase *m_db;
    QList<QMetaProperty> m_lstPropertys;
};



template <typename T>
inline QString SqlHelper<T>::originalProperty(const QString &name)
{
    return name.right(name.length() - QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR).length());
}

template <typename T>
inline QString SqlHelper<T>::tableName()
{
    QMetaObject meta = T::staticMetaObject;
    return meta.className();
}

void test_SqlHelper();

#endif // SqlHelper_H
