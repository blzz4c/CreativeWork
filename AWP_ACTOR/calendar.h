#ifndef CALENDAR_H
#define CALENDAR_H

#include <QWidget>
#include <authorization.h>
#include <string>
#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QJsonObject>
#include <QFileDialog>
#include <QStandardItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDateTimeEdit>
using namespace std;

namespace Ui {
class Calendar;
}

class Calendar : public QWidget
{
    Q_OBJECT

public:
    explicit Calendar(QString m_Log, QWidget *parent);
    ~Calendar();
    QString Log;

protected slots:
    void on_MoveButtonOk_cliked();
private slots:
    void on_MoveButton_clicked();

    void on_AddButton_clicked();

    void on_DelButton_clicked();

    void on_calendarWidget_clicked(const QDate &date);

    void on_SignOutButton_clicked();

    void on_InputDataOkButton_Clicked();

    void on_DelButtonOk_clicked();

    void on_ShowCurrentDatesButton_clicked();

    void on_SearchButton_clicked();

private:
    Ui::Calendar *ui;
    QWidget *window;
    QLineEdit *Input1;
    QDateTimeEdit *Input2;
    void on_ShowFullButton_clicked();
    list<int> NewDatesNumbers;
    QJsonDocument doc;
    QJsonArray docAr;
    QJsonParseError docError;
    QJsonObject UsersDataJson;
    QJsonObject json;
    string CurrentDate = "empty";
};

#endif // CALENDAR_H
