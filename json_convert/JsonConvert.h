#ifndef JsonConvert_H
#define JsonConvert_H

#include <QObject>
#include <QDomDocument>


#define JSON_CONVERT_STR_(x)      #x
#define JSON_CONVERT_STR(x)       JSON_CONVERT_STR_(x)

#define JSON_CONVERT_CONS_(x, y) x##y
#define JSON_CONVERT_CONS(x, y)  JSON_CONVERT_CONS_(x, y)

#define JSON_CONVERT_PROPERTY_NAME_PREFIX                  jsonConvert_
#define JSON_CONVERT_PROPERTY_NAME_PREFIX_STR          JSON_CONVERT_STR(JSON_CONVERT_PROPERTY_NAME_PREFIX)

#define JSON_CONVERT_CREATE_PROPERTY_NAME_PREFIX                  jsonConvertCreate_
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

class JsonConvert : public QObject
{
    Q_OBJECT

public:
    explicit JsonConvert(QObject *parent = nullptr);
    ~JsonConvert();

    bool toObject(const QJsonObject &json, QObject &obj);

    bool toJson(const QObject &obj, QJsonObject &json);

    static void test_toObject();

private:
    void setProperty(QObject &obj, int id, QVariant v);

    QObject *createObject(QObject &obj, const QString &key);
};

#endif // JsonConvert_H
