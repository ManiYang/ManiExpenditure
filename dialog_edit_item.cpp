#include <QMessageBox>
#include "dialog_edit_item.h"
#include "ui_dialog_edit_item.h"

clDialog_EditItem::clDialog_EditItem(const QStringList &categories, QWidget *parent)
    : QDialog(parent), ui(new Ui::clDialog_EditItem)
{
    ui->setupUi(this);

    ui->comboBox_category->addItems(categories);
    ui->comboBox_category->setCurrentText("");

    ui->dateEdit_spending_date->setDate(QDate::currentDate());
    ui->dateEdit_spending_date->setDisplayFormat("yyyy/M/d");
}

clDialog_EditItem::clDialog_EditItem(const QStringList &categories, const clItemData &data,
                                     QWidget *parent)
    : QDialog(parent), ui(new Ui::clDialog_EditItem)
{
    ui->setupUi(this);

    ui->comboBox_category->addItems(categories);
    ui->comboBox_category->setCurrentText("");

    ui->dateEdit_spending_date->setDate(QDate::currentDate());
    ui->dateEdit_spending_date->setDisplayFormat("yyyy/M/d");

    //
    ui->comboBox_category->setCurrentText(data.mCategory);
    ui->lineEdit_item->setText(data.mItemName);
    ui->spinBox_cost->setValue(data.mCost);
    ui->dateEdit_spending_date->setDate(data.mSpendingDate);
    ui->spinBox_year->setValue(data.mDurationYear);
    ui->spinBox_month->setValue(data.mDurationMonth);
    ui->spinBox_day->setValue(data.mDurationDay);
    ui->lineEdit_note->setText(data.mNote);
}

clDialog_EditItem::~clDialog_EditItem()
{
    delete ui;
}

void clDialog_EditItem::get_data(clItemData *data)
{
    data->mCategory = ui->comboBox_category->currentText();
    data->mItemName = ui->lineEdit_item->text();
    data->mCost = ui->spinBox_cost->value();
    data->mSpendingDate = ui->dateEdit_spending_date->date();
    data->mDurationYear = ui->spinBox_year->value();
    data->mDurationMonth = ui->spinBox_month->value();
    data->mDurationDay = ui->spinBox_day->value();
    data->mNote = ui->lineEdit_note->text();
}

void clDialog_EditItem::accept()
{
    if(ui->spinBox_year->value()==0
       && ui->spinBox_month->value()==0
       && ui->spinBox_day->value()==0)
    {
        QMessageBox::warning(this, "bad input", "Duration should not be 0.");
        return;
    }

    //
    QDialog::accept();
}
