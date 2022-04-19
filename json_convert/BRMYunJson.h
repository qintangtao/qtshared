#ifndef _BRMYunJson_H
#define _BRMYunJson_H

#include <QObject>
#include <JsonConvert.h>
#include <QVariantList>


class BRMYunJson  : public QObject {
    Q_OBJECT

    JSON_CONVERT_PROPERTY(QString, cmd_top, WRITE set_cmd_top READ cmd_top)
    JSON_CONVERT_PROPERTY(QString, cmd_bottom, WRITE set_cmd_bottom READ cmd_bottom)
    JSON_CONVERT_PROPERTY(QString, cmd_left, WRITE set_cmd_left READ cmd_left)
    JSON_CONVERT_PROPERTY(QString, cmd_right, WRITE set_cmd_right READ cmd_right)
    JSON_CONVERT_PROPERTY(int, test_int, WRITE set_test_int READ test_int)
    JSON_CONVERT_PROPERTY(double, test_double, WRITE set_test_double READ test_double)
    JSON_CONVERT_PROPERTY(bool, test_bool, WRITE set_test_bool READ test_bool)
    JSON_CONVERT_PROPERTY(QVariantList, list_json, WRITE set_list_json READ list_json)
    JSON_CONVERT_PROPERTY(BRMYunJson *, obj_json, WRITE set_obj_json READ obj_json)

    JSON_CONVERT_CREATE_PROPERTY(BRMYunJson *, list_json, READ create_obj_json)
    JSON_CONVERT_CREATE_PROPERTY(BRMYunJson *, obj_json, READ create_obj_json)

public:
    explicit BRMYunJson(QObject *parent = nullptr);
    ~BRMYunJson() ;

    JSON_CONVERT_METHOD(QString, m_cmd_top, set_cmd_top, cmd_top)
    JSON_CONVERT_METHOD(QString, m_cmd_bottom, set_cmd_bottom, cmd_bottom)
    JSON_CONVERT_METHOD(QString, m_cmd_left, set_cmd_left, cmd_left)
    JSON_CONVERT_METHOD(QString, m_cmd_right, set_cmd_right, cmd_right)
    JSON_CONVERT_METHOD(int, m_test_int, set_test_int, test_int)
    JSON_CONVERT_METHOD(double, m_test_double, set_test_double, test_double)
    JSON_CONVERT_METHOD(bool, m_test_bool, set_test_bool, test_bool)
    JSON_CONVERT_METHOD(QVariantList, m_list_json, set_list_json, list_json)
    JSON_CONVERT_METHOD(BRMYunJson *, m_obj_json, set_obj_json, obj_json)

    JSON_CONVERT_CREATE_METHOD(BRMYunJson, create_obj_json)

    QString toString() const;

private:
    QVariantList m_list_json;
    BRMYunJson *m_obj_json;

    bool m_test_bool;
    int m_test_int;
    double m_test_double;

    QString m_cmd_top;
    QString m_cmd_bottom;
    QString m_cmd_left;
    QString m_cmd_right;

private:
    Q_DISABLE_COPY(BRMYunJson)
};

#endif
