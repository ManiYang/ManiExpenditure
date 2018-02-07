#include <QTableWidgetItem>
#include <QDomDocument>
#include <QDomElement>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog_edit_item.h"
#include "utility_DOM.h"
#include "utility_math.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      mLargestID(-1)
{
    ui->setupUi(this);

    ui->tabWidget->setTabText(0, "Records");
    ui->tabWidget->setTabText(1, "Chart");

    //
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setVisible(false);

    //
    ui->pushButton_edit->setEnabled(false);
    ui->pushButton_remove->setEnabled(false);

    // tab 1: chart
    mChartView = new QChartView(this);
    ui->tab_1->layout()->addWidget(mChartView);

    ui->spinBox_start_year->setValue(QDate::currentDate().year());

    // read data and plot
    mFileName = "expenditure.xml";
    if(!read_file())
        exit(EXIT_FAILURE);

    //
    mSeries = new QLineSeries;
    mSeries->setPointsVisible(true);
    mChartView->chart()->addSeries(mSeries);

    mXAxis = new QDateTimeAxis;
    mXAxis->setFormat("yyyy/MM");

    mYAxis = new QValueAxis;
    mYAxis->setLabelFormat("%d");
    mYAxis->setTickCount(6);

    mChartView->chart()->setAxisX(mXAxis, mSeries);
    mChartView->chart()->setAxisY(mYAxis, mSeries);

    mChartView->chart()->legend()->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_add_clicked()
{
    clDialog_EditItem dialog(mCategories);
    int r = dialog.exec();
    if(r == QDialog::Accepted)
    {
        clItemData data;
        dialog.get_data(&data);

        // add to table
        mLargestID++;
        insert_to_table(-1, mLargestID, data);

        // save to file
        save_file();

        //
        if(! mCategories.contains(data.mCategory))
            mCategories << data.mCategory;
    }
}

void MainWindow::on_actionQuit_triggered()
{
    this->close();
}

bool MainWindow::save_file()
{
    /// prepare XML document
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml",
                                                    "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = doc.createElement("expenditure");
    doc.appendChild(root);

    //
    int Nrow = ui->tableWidget->rowCount();
    for(int r=0; r<Nrow; r++)
    {
        QDomElement elem_item = nsDomUtil::add_child_element(doc, root, "item");

        //
        clItemData data;
        int id;
        get_data_from_table_row(r, &id, &data);

        //
        elem_item.setAttribute("id", QString::number(id));

        //
        nsDomUtil::add_child_element_w_text(doc, elem_item, "category", data.mCategory);
        nsDomUtil::add_child_element_w_text(doc, elem_item, "name", data.mItemName);
        nsDomUtil::add_child_element_w_text(doc, elem_item, "cost",
                                            QString::number(data.mCost));
        nsDomUtil::add_child_element_w_text(doc, elem_item, "spending-date",
                                            data.mSpendingDate.toString("yyyy/M/d"));

        QDomElement elem_dur = nsDomUtil::add_child_element(doc, elem_item, "duration");
        if(data.mDurationYear > 0)
            nsDomUtil::add_child_element_w_text(doc, elem_dur, "year",
                                                QString::number(data.mDurationYear));
        if(data.mDurationMonth > 0)
            nsDomUtil::add_child_element_w_text(doc, elem_dur, "month",
                                                QString::number(data.mDurationMonth));
        if(data.mDurationDay> 0)
            nsDomUtil::add_child_element_w_text(doc, elem_dur, "day",
                                                QString::number(data.mDurationDay));

        if(! data.mNote.isEmpty())
            nsDomUtil::add_child_element_w_text(doc, elem_item, "note", data.mNote);
    }

    /// write to file
    QFile F(mFileName);
    if(!F.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "warning",
                             QString("Coult not open %1 for writing.").arg(mFileName));
        return false;
    }

    F.write(doc.toByteArray(2));
    F.close();
    return true;
}

void MainWindow::get_data_from_table_row(const int row, int *id, clItemData *data)
{
    int Nrow = ui->tableWidget->rowCount();
    Q_ASSERT(0<=row && row<Nrow);

    QTableWidgetItem *item = ui->tableWidget->item(row, 0);
    Q_ASSERT(item != nullptr);
    data->mCategory = item->text();

    bool ok;
    *id = item->data(Qt::UserRole).toInt(&ok);
    Q_ASSERT(ok);

    item = ui->tableWidget->item(row, 1);
    Q_ASSERT(item != nullptr);
    data->mItemName = item->text();

    item = ui->tableWidget->item(row, 2);
    Q_ASSERT(item != nullptr);
    data->mCost = item->data(Qt::UserRole).toInt(&ok);
    Q_ASSERT(ok);

    item = ui->tableWidget->item(row, 3);
    Q_ASSERT(item != nullptr);
    data->mSpendingDate = QDate::fromString(item->text(), "yyyy/MM/dd");
    Q_ASSERT(data->mSpendingDate.isValid());

    item = ui->tableWidget->item(row, 4);
    Q_ASSERT(item != nullptr);
    data->mDurationYear = item->data(Qt::UserRole+0).toInt(&ok);
    Q_ASSERT(ok);
    data->mDurationMonth = item->data(Qt::UserRole+1).toInt(&ok);
    Q_ASSERT(ok);
    data->mDurationDay = item->data(Qt::UserRole+2).toInt(&ok);
    Q_ASSERT(ok);

    item = ui->tableWidget->item(row, 5);
    Q_ASSERT(item != nullptr);
    data->mNote = item->text();
}

bool MainWindow::read_file()
//also build table
{
    QFile F(mFileName);
    if(!F.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "warning",
                             QString("Coult not open %1 for reading.").arg(mFileName));
        return false;
    }

    QDomDocument doc;
    QString error_msg;
    int error_line;
    bool ch = doc.setContent(&F, false, &error_msg, &error_line);
    if(!ch)
    {
        QMessageBox::warning(this, "error",
                             QString("Error in parsing XML file \"%1\".\n\nLine %2: %3.")
                             .arg(mFileName).arg(error_line).arg(error_msg));
        return false;
    }

    //
    QDomElement root = doc.firstChildElement();
    QDomElement elem_item = root.firstChildElement("item");
    for( ; !elem_item.isNull(); elem_item=elem_item.nextSiblingElement("item"))
    {
        int id;
        {
            QString str_id = elem_item.attribute("id");
            bool ok;
            id = str_id.toInt(&ok);
            Q_ASSERT(ok);
        }

        //
        QString category, item_name, note;
        int cost, dur_year, dur_month, dur_day;
        QDate spending_date;

        category = nsDomUtil::get_text_of_first_child_element(elem_item, "category");
        item_name = nsDomUtil::get_text_of_first_child_element(elem_item, "name");

        QString str_cost = nsDomUtil::get_text_of_first_child_element(elem_item, "cost");
        bool ok;
        cost = str_cost.toInt(&ok);
        Q_ASSERT(ok);

        QString str_date
                = nsDomUtil::get_text_of_first_child_element(elem_item, "spending-date");
        spending_date = QDate::fromString(str_date, "yyyy/M/d");
        Q_ASSERT(spending_date.isValid());

        dur_year = dur_month = dur_day = 0;
        QDomElement elem_dur = elem_item.firstChildElement("duration");
        QString str_n = nsDomUtil::get_text_of_first_child_element(elem_dur, "year");
        if(! str_n.isEmpty())
        {
            dur_year = str_n.toInt(&ok);
            Q_ASSERT(ok);
        }
        str_n = nsDomUtil::get_text_of_first_child_element(elem_dur, "month");
        if(! str_n.isEmpty())
        {
            dur_month = str_n.toInt(&ok);
            Q_ASSERT(ok);
        }
        str_n = nsDomUtil::get_text_of_first_child_element(elem_dur, "day");
        if(! str_n.isEmpty())
        {
            dur_day= str_n.toInt(&ok);
            Q_ASSERT(ok);
        }

        note = nsDomUtil::get_text_of_first_child_element(elem_item, "note");

        //
        clItemData data;
        data.set(category, item_name, cost, spending_date,
                 dur_year, dur_month, dur_day, note);
        insert_to_table(-1, id, data);

        //
        if(id > mLargestID)
            mLargestID = id;

        //
        if(! mCategories.contains(category))
            mCategories << category;
    }

    //
    F.close();
    return true;
}


void MainWindow::insert_to_table(const int row_, const int id, const clItemData &data)
//If `row` = -1, append new row.
{
    int Nrow = ui->tableWidget->rowCount();
    int row = row_;
    if(row_ == -1)
        row = Nrow;
    Q_ASSERT(row>=0 && row<=Nrow);

    //
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->insertRow(row);

    QTableWidgetItem *item = new QTableWidgetItem(data.mCategory);
    item->setData(Qt::ToolTipRole, QString::number(id));
    item->setData(Qt::UserRole, id);
    ui->tableWidget->setItem(row, 0, item);

    item = new QTableWidgetItem(data.mItemName);
    ui->tableWidget->setItem(row, 1, item);

    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, QString::number(data.mCost));
    item->setData(Qt::UserRole, data.mCost);
    ui->tableWidget->setItem(row, 2, item);

    item = new QTableWidgetItem;
    item->setData(Qt::DisplayRole, data.mSpendingDate.toString("yyyy/MM/dd"));
    ui->tableWidget->setItem(row, 3, item);

    item = new QTableWidgetItem;
    QString str_dur;
    {
        if(data.mDurationYear > 0)
            str_dur += QString("%1 yr").arg(data.mDurationYear);
        if(data.mDurationMonth > 0)
        {
            if(! str_dur.isEmpty())
                str_dur += " + ";
            str_dur += QString("%1 month").arg(data.mDurationMonth);
        }

        if(! str_dur.isEmpty())
        {
            if(data.mDurationDay > 0)
                str_dur += QString(" + %1 day").arg(data.mDurationDay);
        }
        else
            str_dur += QString("%1 day").arg(data.mDurationDay);
    }
    item->setData(Qt::DisplayRole, str_dur);
    item->setData(Qt::UserRole+0, data.mDurationYear);
    item->setData(Qt::UserRole+1, data.mDurationMonth);
    item->setData(Qt::UserRole+2, data.mDurationDay);
    ui->tableWidget->setItem(row, 4, item);

    item = new QTableWidgetItem(data.mNote);
    ui->tableWidget->setItem(row, 5, item);

    //
    ui->tableWidget->setSortingEnabled(true);
}

void MainWindow::on_tableWidget_currentItemChanged(QTableWidgetItem *current,
                                                   QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    ui->pushButton_edit->setEnabled(current != nullptr);
    ui->pushButton_remove->setEnabled(current != nullptr);
}

void MainWindow::on_pushButton_edit_clicked()
{
    if(ui->tableWidget->currentColumn() < 0)
        return;

    const int row = ui->tableWidget->currentRow();
    if(row < 0)
        return;

    //
    clItemData data;
    int id;
    get_data_from_table_row(row, &id, &data);

    clDialog_EditItem dialog(mCategories, data);
    int r = dialog.exec();
    if(r == QDialog::Accepted)
    {
        dialog.get_data(&data);

        ui->tableWidget->setSortingEnabled(false);
        ui->tableWidget->removeRow(row);
        insert_to_table(row, id, data);

        //
        ui->tableWidget->setCurrentCell(row, 0);
        ui->tableWidget->setSortingEnabled(true);

        //
        save_file();
    }
}

void MainWindow::on_pushButton_remove_clicked()
{
    if(ui->tableWidget->currentColumn() < 0)
        return;

    const int row = ui->tableWidget->currentRow();
    if(row < 0)
        return;

    //
    int r = QMessageBox::question(this, "confirm", "Are you sure to remove the item?");
    if(r != QMessageBox::Yes)
        return;

    //
    ui->tableWidget->removeRow(row);
    save_file();
}

void MainWindow::on_pushButton_duplicate_clicked()
{
    if(ui->tableWidget->currentColumn() < 0)
        return;

    const int row = ui->tableWidget->currentRow();
    if(row < 0)
        return;

    //
    clItemData data;
    int id;
    get_data_from_table_row(row, &id, &data);

    mLargestID++;
    insert_to_table(row+1, mLargestID, data);

    //
    save_file();
}

void MainWindow::plot()
{
    const int start_yr = ui->spinBox_start_year->value();
    const int end_yr = start_yr + ui->spinBox_Nyears->value() - 1;

    // data
    QList<int> monthly_spend;
    for(int yr=start_yr; yr<=end_yr; yr++)
    {
        QList<int> sum_1yr;
        for(int m=0; m<12; m++)
            sum_1yr << 0;

        for(int r=0; r<ui->tableWidget->rowCount(); r++)
        {
            int id;
            clItemData data;
            get_data_from_table_row(r, &id, &data);

            QList<int> spend_1yr = data.get_monthly_expenditure_within_year(yr);

            for(int m=0; m<12; m++)
                sum_1yr[m] += spend_1yr.at(m);
        }

        monthly_spend << sum_1yr;
    }

    // set series
    mSeries->clear();

    int yr = start_yr, mn = 1;
    foreach(int s, monthly_spend)
    {
        QDateTime t(QDate(yr,mn,15),QTime(12,0));
        mSeries->append(t.toMSecsSinceEpoch(), s);

        //
        mn++;
        if(mn == 13)
        {
            yr++;
            mn = 1;
        }
    }

    //
    mXAxis->setMin(QDateTime(QDate(start_yr,1,1),QTime(0,0)));
    mXAxis->setMax(QDateTime(QDate(end_yr,12,31),QTime(23,59)));

    //
    int s_max = maximum_of_list(monthly_spend);
    int y_max = 1250;
    while(y_max <= s_max)
        y_max *= 2;
    mYAxis->setRange(0, y_max);
}

void MainWindow::on_pushButton_plot_clicked()
{
    plot();
}
