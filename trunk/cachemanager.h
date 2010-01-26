#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "netmodel.h"
#include <QMap>
class CacheManager
{
public:
    CacheManager();
    void reset(Event* ev);
    void put(Event *p1,Event *p2, const QList<Path>* data);
    void put(Event *p1,Event *p2, const QList<Path>& data) {put(p1,p2,&data);}
    bool get(Event *p1,Event *p2, QList<Path>* result);
    void prepending(Event *ev,QSet<Event*>&);
    void postponing(Event *ev,QSet<Event*>&);
private:
    typedef QMap<Event*,QList<Path>*> CacheRow;
    typedef QMap<Event*,CacheRow> Cache;
    int hit;
    int miss;
    Cache cache;
};

#endif // CACHEMANAGER_H
