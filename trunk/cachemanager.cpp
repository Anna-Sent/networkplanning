#include "cachemanager.h"
#include <QList>
#include <QDebug>

CacheManager::CacheManager()
{
    hit=0;
    miss=0;
}

void CacheManager::put(Event *p1,Event *p2, const QList<Path>* data)
{
    if (cache[p1][p2]) {
        (*cache[p1][p2])=*data;
    } else {
        cache[p1][p2]=new QList<Path>(*data);
    }
}

bool CacheManager::get(Event *p1,Event *p2, QList<Path>* data)
{
    if (cache[p1][p2]) {
        (*data).append(*cache[p1][p2]);
        ++hit;
        return true;
    } else {
        ++miss;
        return false;
    }
}

void CacheManager::reset(Event *ev)
{
    qDebug() << hit << miss;
    hit=0;
    miss=0;
    foreach(CacheRow row,cache)
    {
        qDeleteAll(row);
    }
    cache.clear();
}
