#include "BRMYunJson.h"

BRMYunJson::BRMYunJson(QObject *parent)
    : QObject(NULL)
    , m_obj_json(NULL){
}

BRMYunJson::~BRMYunJson() {}

QString BRMYunJson::toString() const
{
    return QLatin1String("cmd_top: ") + m_cmd_top +
                QLatin1String(", cmd_bottom: ") + m_cmd_bottom +
                QLatin1String(", cmd_left: ") + m_cmd_left +
                QLatin1String(", cmd_right: ") + m_cmd_right +
                QLatin1String(", test_bool: ") + QString("%1").arg(m_test_bool) +
                QLatin1String(", test_double: ") + QString::number(m_test_double) +
                QLatin1String(", test_int: ") + QString::number(m_test_int) ;
}

QObject* BRMYunJson::newIns(const QString &typeName)
{
    if (typeName == "list_json" ||
            typeName == "obj_json")
        return new BRMYunJson();

    return Q_NULLPTR;
}


QObject* BRMYunJson::newIns2(const QString &typeName)
{
    return new BRMYunJson();
}
