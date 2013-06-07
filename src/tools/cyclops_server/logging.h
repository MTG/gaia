#ifndef GAIA_CYCLOPSLOGGING_H
#define GAIA_CYCLOPSLOGGING_H

#include <QDebug>
#include <QDateTime>

inline QDebug clog() {
  return qDebug() << QString("%1:").arg(QDateTime::currentDateTime().toString())
    .toAscii().constData(); // toAscii.constData to avoid the "..." around QStrings
}

#endif // GAIA_CYCLOPSLOGGING_H
