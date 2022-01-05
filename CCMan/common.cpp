#include <QDateTime>

#include "common.h"

QString getDateTime( time_t tTime )
{
    QDateTime dateTime;
    dateTime.setTime_t( tTime );

    return dateTime.toString( "yyyy-MM-dd HH:mm:ss");
}
