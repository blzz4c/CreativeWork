#include "authorization.h"
#include "ui_authorization.h"
#include "calendar.h"

Authorization::Authorization(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Authorization)
{
    ui->setupUi(this);
    QFile file("./Users.json");
    setWindowTitle("Авторизация");
    file.open(QIODevice::ReadOnly|QFile::Text);
    doc = QJsonDocument::fromJson(QByteArray(file.readAll()), &docError);
    file.close();
    QJsonValue Users = doc.object().value("Users");
    docAr = QJsonValue(Users).toArray();
    json = docAr.at(0).toObject();
    QPixmap bkgnd(":/Background/RNFSLG.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);
    setFixedSize(width(), height());
}

Authorization::~Authorization()
{
    delete ui;
}

void Authorization::ToCalendar() {
    QWidget *m_Calendar = new Calendar(Log, nullptr);
    m_Calendar->setGeometry(geometry());
    m_Calendar->setFixedSize(m_Calendar->width(), m_Calendar->height());
    m_Calendar->show();
    close();
}

bool Authorization::isLogValid()
{
    //Логин не менее 4 символов
    //Содержит хотя бы одну букву
    //Не соддержит пробелов
    //Латиница
    bool Valid = (Log.length() > 3);
    bool ContainsW = false;
    for (int i = 0; i < Log.length() && Valid; i++) {
        Valid = (Log[i] != ' ') && (Log[i] < 127);
        if ((Log[i] > 64 && Log[i] < 91) || (Log[i] > 96  && Log[i] < 123)) {//Находится внутри кодов латинских букв
            ContainsW = true;
        }
    }
    return (Valid && ContainsW);
}

bool Authorization::isPasValid()
{
    //Не менее 8 символов
    //Не содержит пробелов
    bool Valid = (Pas.length() > 7);
    for (int i = 0; i < Log.length() && Valid; i++) {
        Valid = (Log[i] != ' ');
    }
    return Valid;
}


void Authorization::on_RegisterButton_clicked()
{
    QFile file("./Users.json");
    Log = ui->InputLog_2->text();
    Pas = ui->InputPas_2->text();
    if (!isLogValid() && !isPasValid()) {
        char a[1000] = "Логин и пароль не корректны!\n\n"
                      "৹ Логин должен:\n"
                      "৹ Содержать не менее 4 символов\n"
                      "৹ Включать в себя хотя бы одну латинскую букву\n"
                      "৹ Не содержать пробелы\n"
                      "৹ Базироваться на латинице\n\n"
                       "Пароль должен:\n"
                       "৹ Содержать не менее 8 символов\n"
                       "৹ Не включать в себя пробелы";
        QMessageBox::critical(this, "Ошибка", a);
    }
    else if (!isLogValid()) {
        char a[1000] = "Логин не корректен!\n\n"
                      "৹ Логин должен:\n"
                      "৹ Содержать не менее 4 символов\n"
                      "৹ Включать в себя хотя бы одну латинскую букву\n"
                      "৹ Не содержать пробелы\n"
                      "৹ Базироваться на латинице";
        QMessageBox::critical(this, "Ошибка", a);
    }
    else if (!isPasValid()) {
        char a[1000] = "Пароль не корректен!\n\n"
                       "Пароль должен:\n"
                       "৹ Содержать не менее 8 символов\n"
                       "৹ Не включать в себя пробелы";
        QMessageBox::critical(this, "Ошибка", a);
    }
    else if (json.find(Log) == json.end()) {    //проверка на оригинальность логина
        file.open(QIODevice::WriteOnly);
        json.insert(Log, Pas);
        QJsonArray docToWrite = {};
        docToWrite.append(json);
        doc.setArray(docToWrite);
        file.write("{\"Users\":"+doc.toJson()+"}");
        file.close();
        QMessageBox::about(this, "Регистрация", "Регистрация прошла успешно!");
        ToCalendar();
    }
    else {
        QMessageBox::critical(this, "Ошибка", "Логин уже используется!");

    }
}


void Authorization::on_LoginButton_clicked()
{
    Log = ui->InputLog->text();
    Pas = ui->InputPas->text();
    if (json.find(Log) != json.end()) {//если логин существует
        if (json.value(Log) == Pas) {//проверка сходимости паролей
            ToCalendar();
        }
        else {
            QMessageBox::critical(this, "Ошибка", "Логин или пароль не верны!");
        }

    }
    else {
        QMessageBox::critical(this, "Ошибка", "Указанный логин не существует!");
    }

}
