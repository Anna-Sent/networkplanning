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

void CacheManager::prepending(Event *ev,QSet<Event*>& r)
{
    if(ev==0) return;
    if (!r.contains(ev)) {
        r.insert(ev);
        foreach(Operation* o,ev->getInOperations())
        {
            prepending(o->getBeginEvent(),r);
        }
    }
}

void CacheManager::postponing(Event *ev,QSet<Event*>& r)
{
    if(ev==0) return;
    if (!r.contains(ev)) {
        r.insert(ev);
        foreach(Operation* o,ev->getOutOperations())
        {
            postponing(o->getEndEvent(),r);
        }
    }
}

void CacheManager::reset(Event *ev)
{
    qDebug() << hit << miss;
    hit=0;
    miss=0;
    if (ev==0) {
        foreach(CacheRow row,cache)
        {
            qDeleteAll(row);
        }
        cache.clear();
    } else {
        QSet<Event*> pre;
        QSet<Event*> post;
        prepending(ev,pre);
        postponing(ev,post);
        foreach(Event* e1,pre)
        {
            foreach(Event* e2,post)
            {
                QList<Path>* &it = cache[e1][e2];
                delete it;
                it=0;
            }
        }
    }
}
