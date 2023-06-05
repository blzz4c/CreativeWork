#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QMainWindow>
#include <QMessageBox>
#include <QMap>
#include <QFile>
#include <QJsonObject>
#include <QFileDialog>
#include <QStandardItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

QT_BEGIN_NAMESPACE
namespace Ui { class Authorization; }
QT_END_NAMESPACE

class Authorization : public QMainWindow
{
    Q_OBJECT

public:
    Authorization(QWidget *parent = nullptr);
    ~Authorization();
    QJsonDocument doc;
    QJsonArray docAr;
    QJsonParseError docError;
    QJsonObject UsersDataJson;
    QJsonObject json;
private slots:
    void on_RegisterButton_clicked();

    void on_LoginButton_clicked();

private:
    Ui::Authorization *ui;
    void ToCalendar();
    QString Log, Pas;
    QMap<QString, QString> UsersData;
    bool isLogValid();
    bool isPasValid();


};
#endif // AUTHORIZATION_H
