#include "JsonConvert.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVariant>
#include "BRMYunJson.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(logJsonConvert, "log.json.convert")

JsonConvert::JsonConvert(QObject *parent) :
    QObject(parent)
{
    
}

JsonConvert::~JsonConvert()
{
}

bool JsonConvert::toObject(const QJsonObject &json, QObject &obj)
{
    const QMetaObject* meta = obj.metaObject();
    if ( Q_NULLPTR == meta)
        return false;

    qCDebug(logJsonConvert) << "====================Start " << meta->className() << "=====================";

    QStringList keys = json.keys();
    foreach(QString key, keys)
    {
        int id = meta->indexOfProperty(QString("%1%2")
                                       .arg(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR))
                                       .arg(key)
                                       .toLocal8Bit().constData());
        if (-1 == id) {
            qCWarning(logJsonConvert) << "key" << key << "id" << id;
            continue;
        }

        QMetaProperty p = meta->property(id);
        QJsonValue jsonValue = json.value(key);
        QJsonValue::Type jsonType = jsonValue.type();

        qCDebug(logJsonConvert) << "key" << key << "type" << jsonType << "id" << id;

        switch (jsonType) {
        case QJsonValue::Bool:
            setProperty(obj, id, jsonValue.toBool());
            break;
        case QJsonValue::Double: {
            switch (p.type()) {
            case QVariant::Int:
            case QVariant::UInt:
            case QVariant::LongLong:
            case QVariant::ULongLong:
                setProperty(obj, id, jsonValue.toInt());
            break;
            case QVariant::Double:
                setProperty(obj, id, jsonValue.toDouble());
            break;
            }
        }
            break;
        case QJsonValue::String:
            setProperty(obj, id, jsonValue.toString());
            break;
        case QJsonValue::Array:
        {
            if (p.type() == QVariant::List)
            {
                QVariantList list;

                QJsonArray jsonArray = jsonValue.toArray();
                for (int i = 0; i < jsonArray.count(); i++) {

                    QObject *subObj = createObject(obj, key);
                    if (Q_NULLPTR == subObj)
                        return false;

                    if (!toObject(jsonArray.at(i).toObject(), *subObj)) {
                        while (!list.isEmpty())
                            delete list.takeFirst().value<QObject *>();
                        return false;
                    }

                    list.append(QVariant::fromValue(subObj));
                }

                setProperty(obj, id, QVariant::fromValue(list));
            }
        }
            break;
        case QJsonValue::Object:
        {
            QObject *subObj = createObject(obj, key);
            if (Q_NULLPTR == subObj)
                return false;

            if (!toObject(jsonValue.toObject(), *subObj)) {
                delete subObj;
                return false;
            }

           setProperty(obj, id, QVariant::fromValue(subObj));
        }
            break;
        }
    }

    qCDebug(logJsonConvert) << "====================End " << meta->className() << "=====================";

    return true;
}

bool JsonConvert::toJson(const QObject &obj, QJsonObject &json)
{
    const QMetaObject* meta = obj.metaObject();
    if ( Q_NULLPTR == meta)
        return false;

    qCDebug(logJsonConvert) << "====================Start " << meta->className() << "=====================";

    int count = meta->propertyCount();
    for (int i = 0; i < count; i++) {
        QMetaProperty p  = meta->property(i);

#if 0
        qCDebug(logJsonConvert)
            << "id:" << i
            << "name:" << p.name()
            << "typeName:" << p.typeName()
            << "type:" << p.type()
            << "isReadable:" << p.isReadable()
            << "isWritable:" << p.isWritable();
#endif

        if (!p.isReadable() || !p.isWritable())
            continue;

        QString name = p.name();
        if (!name.startsWith(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR))) {
            continue;
        }

        name.remove(0, QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR).length());

        qCDebug(logJsonConvert)
            << "id:" << i
            << "name:" << p.name()
            << "typeName:" << p.typeName()
            << "type:" << p.type()
            << "isReadable:" << p.isReadable()
            << "isWritable:" << p.isWritable();

        QVariant v = p.read(&obj);

        switch (p.userType()) {
        case QVariant::Bool:
            json.insert(name, v.toBool());
            break;
        case QVariant::Int:
            json.insert(name, v.toInt());
            break;
        case QVariant::UInt:
            json.insert(name, (int)v.toUInt());
            break;
        case QVariant::LongLong:
            json.insert(name, v.toLongLong());
            break;
        case QVariant::ULongLong:
            json.insert(name, (qint64)v.toULongLong());
            break;
        case QVariant::Double:
            json.insert(name, v.toDouble());
            break;
        case QVariant::String:
            json.insert(name, v.toString());
            break;
        case QMetaType::QVariant:
        {
            QVariant subV = v.value<QVariant>();
            QJsonObject jsonSubObj;
            if (!toJson(**reinterpret_cast< QObject**>(subV.data()),  jsonSubObj))
                return false;
            json.insert(name, jsonSubObj);
        }
            break;
        case QVariant::List:
        {
            QVariantList list = v.toList();
            if (!list.isEmpty())
            {
                QJsonArray jsonArray;
                foreach(QVariant subV, list)
                {
                    QJsonObject jsonSubObj;
                    if (!toJson(**reinterpret_cast< QObject**>(subV.data()),  jsonSubObj))
                        return false;
                    jsonArray.append(jsonSubObj);
                }
                json.insert(name, jsonArray);
            }
        }
            break;
        default:
        {
#if 1
            QObject* subObj = v.value<QObject*>();
            if (Q_NULLPTR != subObj)
            {
                QJsonObject jsonSubObj;
                if (!toJson(*subObj,  jsonSubObj))
                    return false;
                json.insert(name, jsonSubObj);
            }
#endif
        }
            break;
        }

    }

    qCDebug(logJsonConvert) << "====================End " << meta->className() << "=====================";

    return true;
}

void JsonConvert::test_toObject()
{
    QJsonObject json;
    json.insert("cmd_top", "1");
    json.insert("cmd_bottom", "2");
    json.insert("cmd_left", "3");
    json.insert("cmd_right", "4");
    json.insert("test_int",5);
    json.insert("test_double",5.3);
    json.insert("test_bool", false);

    {
        QJsonObject json2;
        json2.insert("cmd_top", "1");
        json2.insert("cmd_bottom", "2");
        json2.insert("cmd_left", "3");
        json2.insert("cmd_right", "4");
        json2.insert("test_int", 6);
        json2.insert("test_double",4.3);
        json2.insert("test_bool", true);

        json.insert("obj_json", json2);
    }

    QJsonArray array;
    for (int i = 0; i < 0; i++) {

        QJsonObject json2;
        json2.insert("cmd_top", "1");
        json2.insert("cmd_bottom", "2");
        json2.insert("cmd_left", "3");
        json2.insert("cmd_right", "4");
        json2.insert("test_double",5.3+i);
        json2.insert("test_int",i);
        json2.insert("test_bool", false);

        array.append(json2);
    }

    json.insert("list_json", array);

    BRMYunJson obj;

    JsonConvert c;
    c.toObject(json, obj);

    qCDebug(logJsonConvert) << obj.toString();

    if (obj.obj_json())
        qCDebug(logJsonConvert) << obj.obj_json()->toString();

#if 0
    QList<BRMYunJson *> list_json = obj.list_json();
    foreach(BRMYunJson *obj, list_json)
    {
        qCDebug(logJsonConvert) << obj->toString();
    }
#endif

     QJsonObject newJson;
    if (c.toJson(obj, newJson))
    {
        QJsonDocument document;
        document.setObject(newJson);
        QByteArray byte_array = document.toJson(QJsonDocument::Compact);
        QString cmdValue = QString::fromLocal8Bit(byte_array);

        qCDebug(logJsonConvert) << cmdValue;
    }

}

void JsonConvert::setProperty(QObject &obj, int id, QVariant v)
{
    int status = -1;
    int flags = 0;
    void *argv[] = { v.data(), &v, &status, &flags };

#if 0
    if (t == QMetaType::QVariant)
        argv[0] = &v;
    else
        argv[0] = v.data();
#endif

    obj.qt_metacall(QMetaObject::WriteProperty, id, argv);
    qCDebug(logJsonConvert) << id << v;
}

QObject *JsonConvert::createObject(QObject &obj, const QString &key)
{
    const QMetaObject* meta = obj.metaObject();
    if ( Q_NULLPTR == meta)
        return Q_NULLPTR;

    int id = meta->indexOfProperty(QString("%1%2%3")
                                   .arg(QStringLiteral(JSON_CONVERT_PROPERTY_NAME_PREFIX_STR))
                                   .arg("new_")
                                   .arg(key)
                                   .toLocal8Bit().constData());
    if (-1 == id) {
        qCWarning(logJsonConvert()) << "className" << meta->className()
                                    << "key" << key
                                    << "id" << id;
        return Q_NULLPTR;
    }

    qCDebug(logJsonConvert()) << "className" << meta->className()
                                << "key" << key
                                << "id" << id;

    QMetaProperty p = meta->property(id);
    QVariant v =  p.read(&obj);
    return  *reinterpret_cast< QObject**>(v.data());
}
