#include "calendar.h"
#include "ui_calendar.h"
#include <QPainter>
#include <QFont>
#include <QTableWidget>
#include <film.h>

Calendar::Calendar(QString m_Log, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calendar)
{
    Log = m_Log;
    ui->setupUi(this);
    setWindowTitle("Календарь");
    //считывание данных из файла
    QFile file("./Calendar" + Log + ".json");
    file.open(QIODevice::ReadOnly|QFile::Text);
    doc = QJsonDocument::fromJson(QByteArray(file.readAll()), &docError);
    file.close();
    QJsonValue Dates = doc.object().value("Dates");
    docAr = QJsonValue(Dates).toArray();
    json = docAr.at(0).toObject();
    //Выделение дат в календаре
    for (QString Date : json.keys()) {
        QTextCharFormat Format;
        QFont f = QFont("MS Shell Dig 2", 14);
        f.setOverline(true);
        Format.setFont(f);
        ui->calendarWidget->setDateTextFormat(QDate::fromString(QString::fromStdString(Date.toStdString().substr(0, 10)),"yyyy.MM.dd"), Format);
    }
    //вывод актуальных дат в таблицу
    on_ShowCurrentDatesButton_clicked();
    for(int i = 0; i < 2; i++) {//важно, если будет resize, то нужно как-то менять размер?
        ui->tableWidget->setColumnWidth(i, (width() - 38)/2);
    }

}

Calendar::~Calendar()
{
    delete ui;
}

//Окно перехода на страницу мероприятия
void Calendar::on_MoveButton_clicked()
{
    window = new QWidget;
    Input1 = new QLineEdit;
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Введите номер ячейки, для перехода к странице мероприятия: ")), 0, 0);
    layout->addWidget(Input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Ввод данных");

    connect(okButton, &QPushButton::clicked, this, &Calendar::on_MoveButtonOk_cliked);
    window->show();
}

//Переход на страницу мероприятия
void Calendar::on_MoveButtonOk_cliked() {
    if ((Input1->text().toInt() > 1) && (Input1->text().toInt() - 1 < ui->tableWidget->rowCount())) {
        window->close();
        QWidget *m_Film = new Film(Log, ui->tableWidget->item(Input1->text().toInt() - 1, 0)->text(), nullptr);
        m_Film->setGeometry(geometry());
        m_Film->setWindowTitle("Мероприятие");
        m_Film->setFixedSize(m_Film->width(), m_Film->height());
        m_Film->show();
        close();
    }
}

//окно добавления данных
void Calendar::on_AddButton_clicked()
{
    window = new QWidget;
    Input1 = new QLineEdit;
    Input2 = new QDateTimeEdit;
    Input2->setDateTime(QDateTime::currentDateTime());
    Input2->setDisplayFormat("yyyy.MM.dd HH:mm");
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Название:")), 0, 0);
    layout->addWidget(Input1, 0, 1);
    layout->addWidget(new QLabel(tr("Дата и время:")), 1, 0);
    layout->addWidget(Input2, 1, 1);
    layout->addWidget(okButton, 2, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Ввод данных");

    connect(okButton, &QPushButton::clicked, this, &Calendar::on_InputDataOkButton_Clicked);
    window->show();

}

//окно удаления данных
void Calendar::on_DelButton_clicked()
{
    window = new QWidget;
    Input1 = new QLineEdit;
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Введите номер ячейки, которую нужно удалить: ")), 0, 0);
    layout->addWidget(Input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Ввод данных");

    connect(okButton, &QPushButton::clicked, this, &Calendar::on_DelButtonOk_clicked);
    window->show();
}

//удаление данных
void Calendar::on_DelButtonOk_clicked() {
    if ((Input1->text().toInt() > 1) && (Input1->text().toInt() - 1 < ui->tableWidget->rowCount())) {
        auto key = ui->tableWidget->item(Input1->text().toInt() - 1, 1)->text();
        auto value = ui->tableWidget->item(Input1->text().toInt() - 1, 0)->text();
        ui->tableWidget->removeRow(Input1->text().toInt());
        json.remove(key);
        //проверка, остались ли мероприятия на текщую дату
        bool ContainsD = false;
        for (auto i : json.keys()) {
            if (i.toStdString().substr(0, 10) == key.toStdString().substr(0, 10)) {
                    ContainsD = true;
            }
        }
        //если у даты больще нет мероприятий возращаем дате привычный вид в календаре
        if (!ContainsD) {
            QTextCharFormat Format;
            QFont f = QFont("MS Shell Dig 2", 8);
            f.setOverline(false);
            Format.setFont(f);
            ui->calendarWidget->setDateTextFormat(QDate::fromString(QString::fromStdString(key.toStdString().substr(0, 10)), "yyyy.MM.dd"), Format);
        }
        //запись json в файл
        QFile file("./Calendar" + Log + ".json");
        file.open(QIODevice::WriteOnly);
        QJsonArray docToWrite = {};
        docToWrite.append(json);
        doc.setArray(docToWrite);
        file.write("{\"Dates\":"+doc.toJson()+"}");
        file.close();
        if (CurrentDate != "empty") {
            on_calendarWidget_clicked(QDate::fromString(QString::fromStdString(key.toStdString().substr(0, 10)), "yyyy.MM.dd"));
        }
        else {
            on_ShowCurrentDatesButton_clicked();
        }
    }
}

//нажатие на дату в календаре
void Calendar::on_calendarWidget_clicked(const QDate &date)
{
    CurrentDate = date.toString("yyyy.MM.dd").toStdString();
    ui->tableWidget->setRowCount(1);
    for (auto i = json.begin(); i != json.end(); i++) {
        if (i.key().toStdString().substr(0, 10) == CurrentDate) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(i.value().toString()));
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(i.key()));
        }
    }
}

//вывод актуальных дат в таблицу
void Calendar::on_ShowCurrentDatesButton_clicked() {
    CurrentDate = "empty";//установка дата не выбрана
    string m_date = QDate::currentDate().toString("yyyy.MM.dd").toStdString();
    ui->tableWidget->setRowCount(json.count() + 1);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("Название"));
    ui->tableWidget->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem("Дата и время"));
    ui->tableWidget->item(0, 1)->setTextAlignment(Qt::AlignCenter);
    int j = 1;
    for (auto i = json.begin(); i != json.end(); i++, j++) {
        if (i.key().toStdString().substr(0, 10) >= m_date) {
            ui->tableWidget->setItem(j, 0, new QTableWidgetItem(i.value().toString()));
            ui->tableWidget->setItem(j, 1, new QTableWidgetItem(i.key()));
        }
    }
}

//выход из аккаунта
void Calendar::on_SignOutButton_clicked()
{
    QWidget *m_Authortization = new Authorization;
    m_Authortization->setGeometry(geometry());
    m_Authortization->setFixedSize(m_Authortization->width(), m_Authortization->height());
    m_Authortization->show();
    window->close();
    close();
}

//добавление данных
void Calendar::on_InputDataOkButton_Clicked()
{
    if (Input1->text() != "" && json.find(Input2->text()) == json.end()) {//если дата с текущим временем существует, то добавить данные нельзя
        //добалвение в Json
        json.insert(Input2->text(), Input1->text());
        //запись json в файл
        QFile file("./Calendar" + Log + ".json");
        file.open(QIODevice::WriteOnly);
        QJsonArray docToWrite = {};
        docToWrite.append(json);
        doc.setArray(docToWrite);
        file.write("{\"Dates\":"+doc.toJson()+"}");
        file.close();
        //Вызов нужного метода
        if (Input2->text().toStdString().substr(0, 10) == CurrentDate) {//если выбрана текущая дата и даты совпадают
                on_calendarWidget_clicked(QDate::fromString(QString::fromStdString(CurrentDate), "yyyy.MM.dd"));
        }
        else {
            on_ShowCurrentDatesButton_clicked();
            //Выделение даты в календаре, если дата не выделена
            if (!ui->calendarWidget->dateTextFormat().value(Input2->date()).font().overline()) {
                QTextCharFormat Format;
                QFont f = QFont("MS Shell Dig 2", 14);
                f.setOverline(true);
                Format.setFont(f);
                ui->calendarWidget->setDateTextFormat(Input2->date(), Format);
            }
        }
    }
    else {
        char a[500] = "Дата с текущим временем уже существует\n"
                      "или введена пустая строка";
        QMessageBox::warning(this, "Ошибка", a);
    }
}

void Calendar::on_SearchButton_clicked()
{
    ui->lineEditSearch->setText("");
    ui->tableWidget->setRowCount(1);
    for (auto i = json.begin(); i != json.end(); i++) {
        if (i.value().toString() == ui->lineEditSearch->text()) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(i.value().toString()));
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(i.key()));
        }
    }
}

