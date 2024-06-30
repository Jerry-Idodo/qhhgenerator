#ifndef HHGEN_H
#define HHGEN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class hhgen;
}
QT_END_NAMESPACE

class hhgen : public QMainWindow
{
    Q_OBJECT

public:
    hhgen(QWidget *parent = nullptr);
    ~hhgen();

private slots:
    void on_generateButton_clicked();

private:
    Ui::hhgen *ui;
};
#endif // HHGEN_H
