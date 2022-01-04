#include "ListAppender.h"

ListAppender::ListAppender(QObject *parent) :
    WriterAppender(parent),
    m_bConfiguratorList(false),
    m_nMaxCount(0)
{
}

ListAppender::~ListAppender()
{

}

void ListAppender::setMaxCount(int n)
{
    if (n < 0)
    {
        //logger()->warn("Attempt to set maximum count for appender '%1' to %2. Using zero instead", name(), n);
        n = 0;
    }

    m_nMaxCount = n;
    ensureMaxCount();
}

QList<QString> ListAppender::clearList()
{
    QList<QString> result = m_list;
    m_list.clear();
    return result;
}

void ListAppender::write(const QString &message)
{
    if ((m_nMaxCount <= 0) || (m_list.size() < m_nMaxCount))
        m_list << message;
}

void ListAppender::ensureMaxCount()
{
    if (m_nMaxCount <= 0)
        return;

    while (m_list.size() > m_nMaxCount)
        m_list.removeFirst();
}
