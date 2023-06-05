#include "film.h"
#include "ui_film.h"
#include <QFile>

Film::Film(QString m_Log, QString m_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Film)
{
    ui->setupUi(this);
    Log = m_Log;
    name = m_name;
    ui->nameLabel->setText(name);
    QFile file("./" + Log + name + ".txt");
    file.open(QIODevice::ReadOnly|QFile::Text);
    QString information = file.readAll();
    file.close();
    ui->textEdit->setText(information);
}

Film::~Film()
{
    delete ui;
}

void Film::on_SaveButton_clicked()
{
    QFile file("./" + Log + name + ".txt");
    file.open(QIODevice::WriteOnly);
    file.write(ui->textEdit->toPlainText().toStdString().data());
    file.close();
}


void Film::on_BackButton_clicked()
{
    QWidget *m_Calendar = new Calendar(Log, nullptr);
    m_Calendar->setGeometry(geometry());
    m_Calendar->show();
    close();
}

