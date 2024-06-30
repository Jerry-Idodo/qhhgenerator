#include "hhgen.h"
#include "./ui_hhgen.h"
#include "hhgen_calcs.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDate>
#include <vector>


hhgen::hhgen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::hhgen)
{
    ui->setupUi(this);
}

hhgen::~hhgen()
{
    delete ui;
}


void user_message(const QString text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void save_hhdata(std::vector<std::vector<double>>& HHData,
                 std::vector<QDate>& Dates)
{
    QString delim{","};
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save CSV File", "", "CSV Files (*.csv);;All Files (*)");
    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (int i = 0; i < Dates.size(); i++) {

        /* Header Row */
        if (i == 0) {
            for (int j = 0; j < max_hours - 1; j++) {
                if (j == 0)
                    out << "Date" << delim;
                out << QString::fromStdString(HalfHourlyTimes[i]) << delim;
            }
            out << QString::fromStdString(HalfHourlyTimes[max_hours - 1]) << "\n";
        }

        /* Date Column */
        out << Dates[i].toString(Qt::ISODate) << delim;

        /* Main Data */
        for (int j = 0; j < max_hours - 1; j++) {
            out << HHData[i][j] << delim;
        }
        out << HHData[i][max_hours - 1] << "\n";
    }
}

void hhgen::on_generateButton_clicked()
{
    static constexpr double max_usage = 10e10;
    bool ok;
    std::vector<QDate> Dates;
    std::vector<std::vector<std::string>> OpenTimesStr;
    std::vector<std::vector<double>> HHData;

    QString start_date_str = hhgen::ui->start_lnedit->text();
    QDate start_date = QDate::fromString(start_date_str, "dd/MM/yyyy");
    if (!start_date.isValid()) {
        user_message("Invalid Start Date!");
        return;
    }
    Dates.push_back(start_date);

    ok = false;
    double annual_usage = ui->usage_lnedit->text().toDouble(&ok);
    if (!ok) {
        user_message("Invalid Annual Usage");
        return;
    }
    if (0 >= annual_usage || annual_usage > max_usage) {
        user_message("Annual Usage out of range!");
        return;
    }

    ok = false;
    double baseload = ui->base_lnedit->text().toDouble(&ok);
    if (!ok) {
        user_message("Invalid Baseload");
        return;
    }
    if (!(0 <= baseload && baseload <= 100)) {
        user_message("Baseload out of range!");
        return;
    }

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        std::vector<std::string> buff;
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            buff.push_back(ui->tableWidget->item(i, j)->text().toStdString());
        }
        OpenTimesStr.push_back(buff);
    }

    ok = false;
    gen_peak_offpeak_data(HHData, Dates, OpenTimesStr, annual_usage, baseload, &ok);
    if (!ok) {
        user_message("Invalid Opening/Closing Times");
        return;
    }

    user_message("Input OK!");

    save_hhdata(HHData, Dates);
}

