#ifndef ITEM_DATA_H
#define ITEM_DATA_H

#include <QString>
#include <QDate>

class clItemData
{
public:
    clItemData() {}

    void set(const QString &category, const QString &item_name,
             const int cost, const QDate &spending_date,
             const int dur_year, const int dur_month, const int dur_day,
             const QString &note);

    void get(QString &category, QString &item_name,
             int &cost, QDate &spending_date,
             int &dur_year, int &dur_month, int &dur_day,
             QString &note) const;

    QList<int> get_monthly_expenditure_within_year(const int year);

    // data:
    QString mCategory;
    QString mItemName;
    int mCost;
    QDate mSpendingDate;
    int mDurationYear;
    int mDurationMonth;
    int mDurationDay;
    QString mNote;

private:
    int mUsageDayCount;
    QDate mExpireDate;
    int get_usage_day_count_in_month(const int year, const int month) const;
};

#endif // ITEM_DATA_H
