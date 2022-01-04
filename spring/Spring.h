#ifndef SPRING_H
#define SPRING_H
#include <QObject>
#include <QVariant>
#include <QMap>


class Spring
{
public:

    static bool install(const QString &name, const QVariant &variant);
    static bool uninstall(const QString &name);

    static bool install(const QString &name, QObject *obj);
    static bool install(QObject *obj);
    static bool uninstall(QObject *obj);

    static QVariant get(const QString &name);
    static QVariant get(const QMetaObject &meta);

    template <class T>
    static T *get(const QString &name) {
        QVariant v = get(name);
        if (v.isValid()) return *reinterpret_cast< T**>(v.data());
        return Q_NULLPTR;
    }

    template <class T>
    static T *get() {
        QVariant v = get(T::staticMetaObject);
        if (v.isValid()) return *reinterpret_cast< T**>(v.data());
        return Q_NULLPTR;
    }

    static bool autowired(QObject* obj);
    static void autowired();

private:
    static QMap<QString, QVariant> m_mapVariants;

private:
    explicit Spring(){}
    ~Spring(){}
};

#endif // SPRING_H
