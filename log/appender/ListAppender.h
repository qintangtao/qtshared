#ifndef LISTAPPENDER_H
#define LISTAPPENDER_H

#include "WriterAppender.h"

class ListAppender :  public WriterAppender
{
    Q_OBJECT

    Q_PROPERTY(bool configuratorList READ configuratorList WRITE setConfiguratorList)
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)

public:
    explicit ListAppender(QObject *parent = NULL);
    ~ListAppender();

private:
    ListAppender(const ListAppender &rOther); // Not implemented
    ListAppender &operator=(const ListAppender &rOther); // Not implemented

public:
    void setConfiguratorList(bool isConfiguratorList);
    bool configuratorList() const;

    void setMaxCount(int n);
    int maxCount() const;

    QList<QString> list() const;
    QList<QString> clearList();

public:
     virtual void write(const QString &message);

private:
    void ensureMaxCount();

private:
    bool m_bConfiguratorList;
    QList<QString> m_list;
    int m_nMaxCount;
};

inline bool ListAppender::configuratorList() const
{   return m_bConfiguratorList;   }

inline int ListAppender::maxCount() const
{   return m_nMaxCount;    }

inline void ListAppender::setConfiguratorList(bool isConfiguratorList)
{   m_bConfiguratorList = isConfiguratorList;   }

inline QList<QString> ListAppender::list() const
{ return m_list; }

#endif // LISTAPPENDER_H
