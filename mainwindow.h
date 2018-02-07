#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QTableWidgetItem>
#include <QtCharts>
using namespace QtCharts;
#include "item_data.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_add_clicked();
    void on_pushButton_duplicate_clicked();
    void on_pushButton_edit_clicked();
    void on_pushButton_remove_clicked();
    void on_tableWidget_currentItemChanged(QTableWidgetItem *current,
                                           QTableWidgetItem *previous);
    void on_actionQuit_triggered();

    void on_pushButton_plot_clicked();

private:
    Ui::MainWindow *ui;

    int mLargestID;
    QString mFileName;
    QStringList mCategories;

    QChartView *mChartView;
    QLineSeries *mSeries;
    QDateTimeAxis *mXAxis;
    QValueAxis *mYAxis;

    //
    bool read_file(); //also build table
    bool save_file();

    void insert_to_table(const int row, //-1: append new row.
                         const int id, const clItemData &data);
    void get_data_from_table_row(const int row, int *id, clItemData *data);

    void plot();
};

#endif // MAINWINDOW_H
