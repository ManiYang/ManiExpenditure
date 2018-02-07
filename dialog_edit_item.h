#ifndef DIALOG_ADD_ITEM_H
#define DIALOG_ADD_ITEM_H

#include <QDialog>
#include <QString>
#include <QDate>
#include "item_data.h"

namespace Ui {
class clDialog_EditItem;
}

class clDialog_EditItem : public QDialog
{
    Q_OBJECT

public:
    explicit clDialog_EditItem(const QStringList &categories, QWidget *parent = nullptr);
    clDialog_EditItem(const QStringList &categories, const clItemData &data,
                      QWidget *parent = nullptr);
    ~clDialog_EditItem();

    void get_data(clItemData *data);

public slots:
    void accept();

private:
    Ui::clDialog_EditItem *ui;
};

#endif // DIALOG_ADD_ITEM_H
