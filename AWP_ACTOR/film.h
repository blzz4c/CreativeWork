#ifndef FILM_H
#define FILM_H

#include <QWidget>
#include <calendar.h>

namespace Ui {
class Film;
}

class Film : public QWidget
{
    Q_OBJECT

public:
    explicit Film(QString m_Log, QString m_name, QWidget *parent);
    ~Film();

private slots:
    void on_SaveButton_clicked();

    void on_BackButton_clicked();

private:
    Ui::Film *ui;
    QString Log;
    QString name;
};

#endif // FILM_H
