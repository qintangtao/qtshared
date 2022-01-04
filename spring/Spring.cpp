#include "Spring.h"
#include <QMetaProperty>
#include <QDebug>
#include "SpringGlobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QLoggingCategory>
Q_LOGGING_CATEGORY(logSpring, "log.spring.core")
#endif

QMap<QString, QVariant>  Spring::m_mapVariants;

static QString qualifiedName(const QMetaObject* meta)
{
    if (Q_NULLPTR == meta)
        return QString();

    QString className = meta->className();
    QString firstword = className.left(1).toLower();
    QString name = firstword + className.mid(1);
    qCDebug(logSpring) << className <<  firstword << name;

    return name;
}

bool Spring::install(const QString &name, const QVariant &variant)
{
    if (!m_mapVariants.contains(name)) {
        qCDebug(logSpring) << name << variant;
        m_mapVariants.insert(name, variant);
        return true;
    }
    return false;
}

bool Spring::uninstall(const QString &name)
{
    if (m_mapVariants.contains(name)) {
        qCDebug(logSpring) << name << m_mapVariants.value(name);
        m_mapVariants.remove(name);
        return true;
    }
    return false;
}

bool Spring::install(const QString &name, QObject *obj)
{
    return Spring::install(name, QVariant::fromValue(obj));
}

bool Spring::install(QObject *obj)
{
    if (Q_NULLPTR == obj)
        return false;

    QString name = qualifiedName(obj->metaObject());
    if (name.isEmpty())
        return false;

    return Spring::install(name, obj);
}

bool Spring::uninstall(QObject *obj)
{
    if (Q_NULLPTR == obj)
        return false;

    QString name = qualifiedName(obj->metaObject());
    if (name.isEmpty())
        return false;

    return Spring::uninstall(name);
}

QVariant Spring::get(const QString &name)
{
    if (m_mapVariants.contains(name)) {
        return m_mapVariants.value(name);
    }
    return QVariant(QVariant::Invalid);
}

QVariant Spring::get(const QMetaObject &meta)
{
    return Spring::get(qualifiedName(&meta));
}

bool Spring::autowired(QObject* obj)
{
    int status = -1;
    int flags = 0;

    if (Q_NULLPTR == obj)
        return false;

    const QMetaObject* meta = obj->metaObject();
    if ( Q_NULLPTR == meta)
        return false;

    qCDebug(logSpring) << "====================Start " << meta->className() << "=====================";

#if 1
    int count = meta->propertyCount();
    for (int i = 0; i < count; i++) {
        QMetaProperty p  = meta->property(i);

        qCDebug(logSpring)
            << "id:" << i
            << "name:" << p.name()
            << "typeName:" << p.typeName()
            << "type:" << p.type()
            << "isWritable:" << p.isWritable();

        if (!p.isWritable())
            continue;

        QString name = p.name();
        if (!name.startsWith(QStringLiteral(SPRING_PROPERTY_NAME_PREFIX_STR))) {
            continue;
        }

        name.remove(0, QStringLiteral(SPRING_PROPERTY_NAME_PREFIX_STR).length());
        if (!m_mapVariants.contains(name)) {
            qCWarning(logSpring)
                << "className:" << meta->className()
                << "id:" << i
                << "name:" << p.name()
                << "newName:" << name
                << "typeName:" << p.typeName()
                << "type:" << p.type()
                << "isWritable:" << p.isWritable()
                << "is no exist";
            continue;
        }

        QVariant::Type t = p.type();
        QVariant  v = m_mapVariants.value(name);

        qCDebug(logSpring) << v;

        void *argv[] = { 0, &v, &status, &flags };
        if (t == QMetaType::QVariant)
            argv[0] = &v;
        else
            argv[0] = v.data();
        obj-> qt_metacall(QMetaObject::WriteProperty, i, argv);
    }

#else
    foreach (QString name, m_mapObjects.keys()) {
        int id = meta->indexOfProperty(name.toLocal8Bit().constData());
        if (id < 0)
            continue;

        QObject* obj2 = m_mapObjects.value(name);
        qCDebug(logSpring)  << name << obj2;

#if 1
        QVariant  v = QVariant::fromValue(obj2);
        void *argv[] = { 0, &v, &status, &flags };
        argv[0] = v.data();
        obj-> qt_metacall(QMetaObject::WriteProperty, id, argv);
#else
        //类型不一样，设置失败
        obj->setProperty(name.toLocal8Bit().constData(), QVariant::fromValue(m_mapObjects.value(name)));
#endif
    }
#endif

    qCDebug(logSpring) << "====================End " << meta->className() << "=====================";
    return true;
}


void Spring::autowired()
{
    foreach (QVariant v, m_mapVariants.values()) {

        qCDebug(logSpring)  << v << v.canConvert(39);

        if (!v.canConvert(39))
            continue;

        autowired(*reinterpret_cast<QObject**>(v.data()));
    }
}
