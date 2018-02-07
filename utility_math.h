#ifndef UTILITY_MATH_H
#define UTILITY_MATH_H

#include <QList>

template <class T>
T maximum_of_list(const QList<T> &list)
//Type `T` must have operator `>` defined.
//`list` cannot be empty.
{
    Q_ASSERT(! list.isEmpty());
    T max =list.at(0);
    for(int i=1; i<list.size(); i++)
    {
        if(list.at(i) > max)
            max = list.at(i);
    }
    return max;
}

#endif // UTILITY_MATH_H
