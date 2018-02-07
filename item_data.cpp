#include <cmath>
#include <QDebug>
#include "item_data.h"

void clItemData::set(const QString &category, const QString &item_name,
                     const int cost, const QDate &spending_date,
                     const int dur_year, const int dur_month, const int dur_day,
                     const QString &note)
{
    mCategory = category;
    mItemName = item_name;
    mCost = cost;
    mSpendingDate = spending_date;
    mDurationYear = dur_year;
    mDurationMonth = dur_month;
    mDurationDay = dur_day;
    mNote = note;

    //
    mUsageDayCount
            = static_cast<int>(std::floor(mDurationYear*365.25 +
                                          mDurationMonth*30.4375 + mDurationDay + 0.5));
    Q_ASSERT(mUsageDayCount > 0);
    mExpireDate = mSpendingDate.addDays(mUsageDayCount-1);
}


void clItemData::get(QString &category, QString &item_name,
                     int &cost, QDate &spending_date,
                     int &dur_year, int &dur_month, int &dur_day,
                     QString &note) const
{
    category = mCategory;
    item_name = mItemName;
    cost = mCost;
    spending_date = mSpendingDate;
    dur_year = mDurationYear;
    dur_month = mDurationMonth;
    dur_day = mDurationDay;
    note = mNote;
}

QList<int> clItemData::get_monthly_expenditure_within_year(const int year)
{
    mUsageDayCount
            = static_cast<int>(std::floor(mDurationYear*365.25 +
                                          mDurationMonth*30.4375 + mDurationDay + 0.5));
    Q_ASSERT(mUsageDayCount > 0);
    mExpireDate = mSpendingDate.addDays(mUsageDayCount-1);

    //
    QList<int> result;
    for(int mn=1; mn<=12; mn++)
    {
        int Nd = get_usage_day_count_in_month(year, mn);
        result << static_cast<int>(std::floor(double(Nd)/mUsageDayCount*mCost + 0.5));
    }
    return result;
}


int clItemData::get_usage_day_count_in_month(const int year, const int month) const
{
    Q_ASSERT(mSpendingDate.isValid());

    const QDate start_of_queried_month(year, month, 1);
    Q_ASSERT(start_of_queried_month.isValid());
    const QDate end_of_queried_month(year, month, start_of_queried_month.daysInMonth());

    if(end_of_queried_month < mSpendingDate)
        return 0;
    else if(start_of_queried_month > mExpireDate)
        return 0;
    else if(end_of_queried_month > mExpireDate)
        return start_of_queried_month.daysTo(mExpireDate) + 1;
    else if(start_of_queried_month < mSpendingDate)
        return mSpendingDate.daysTo(end_of_queried_month) + 1;
    else
        return start_of_queried_month.daysInMonth();
}
