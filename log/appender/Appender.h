#ifndef APPENDER_H
#define APPENDER_H
#include <QObject>

class Appender : public QObject
{
    Q_OBJECT

public:
        Appender(QObject *pParent = 0);
        virtual ~Appender();

private:
        Appender(const Appender &rOther); // Not implemented
        Appender &operator=(const Appender &rOther); // Not implemented

public:
    virtual QString name() const = 0;
    virtual void setName(const QString &rName) = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void write(const QString &message) = 0;
};

inline Appender::Appender(QObject *pParent) :
       QObject(pParent)
    {}

inline Appender::~Appender()
{}

#endif // APPENDER_H
