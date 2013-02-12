#include "netmodel.h"
#include <QDebug>
#include <limits>
#include "cachemanager.h"
using namespace std;

Event::Event()
{
    n = 0;
    point.setX(0);
    point.setY(0);
}

Event::Event(int n)
{
    this->n = n;
    point.setX(0);
    point.setY(0);
}

void Event::addInOperation(Operation *operation)
{
    inputOperations << operation;
}

void Event::addOutOperation(Operation *operation)
{
    outputOperations << operation;
}

bool Event::hasEdge(Event *e)
{
    foreach (Operation *o, outputOperations)
    {
        if (o->getEndEvent()==e)
            return true;
    }
    return false;
}

int Event::getN() const
{
    return n;
}

QList<Operation*>& Event::getInOperations()
{
    return inputOperations;
}

QList<Operation*>& Event::getOutOperations()
{
    return outputOperations;
}

Operation::Operation() :
        beginEvent(NULL), endEvent(NULL), tmin(0), tmax(0), twait(0), _inCriticalPath(false)
{
}

Operation::Operation(double twait) :
        beginEvent(NULL), endEvent(NULL), tmin(0), tmax(0), twait(twait), _inCriticalPath(false)
{
}

Event *Operation::getBeginEvent() const
{
    return beginEvent;
}

Event *Operation::getEndEvent() const
{
    return endEvent;
}

void Operation::setBeginEvent(Event* e)
{
    beginEvent=e;
}

void Operation::setEndEvent(Event* e)
{
    endEvent=e;
}

double Operation::getWaitTime()
{
    return twait;
}

QString Operation::getCode() const
{
    QString first=beginEvent?beginEvent->formatted():Event::emptyFormatted();
    QString second=endEvent?endEvent->formatted():Event::emptyFormatted();
    return first+Event::divider()+second;
}

Path::Path(QList<Event*> events)
{
    this->events = events;
    _weight = calcWeight();
    _code = calcCode();
}

QString Path::calcCode() const
{
    QString s;
    s.reserve(512);
    int count = events.count();
    if (count>0)
    {
        for (int i=0;i<count-1;++i)
        {
            Event *event = events[i];
            s += event->formatted() + Event::divider();
        }
        s += events.last()->formatted();
    }
    return s;
}

double Path::calcWeight() const
{
    double weight=0;
    for (int i=0;i<events.count()-1;++i)
    {
        foreach (Operation *o, events[i]->getOutOperations())
        {
            if (o->getEndEvent()==events[i+1])
            {
                weight += o->getWaitTime();
                break;
            }
        }
    }
    return weight;
}

NetModel::NetModel() : fullPathes(NULL), criticPathes(NULL)
{
    QObject::connect(this, SIGNAL(updated()), this, SLOT(updateCriticalPath()));
    cmanager = new CacheManager();
}

void NetModel::updateCriticalPath()
{
    clearCache();
    //fullPathes = _getFullPathes();
    //criticPathes = _getCriticalPathes();
    foreach (Operation *o, operations)
    {
        o->_inCriticalPath = inCriticalPath(o);
    }
}

NetModel::~NetModel()
{
    clearCache();
    qDeleteAll(events);
    events.clear();
    qDeleteAll(operations);
    operations.clear();
    delete cmanager;
}

void NetModel::clearCache()
{
    if (fullPathes)
    {
        delete fullPathes;
        fullPathes = NULL;
    }
    if (criticPathes)
    {
        delete criticPathes;
        criticPathes = NULL;
    }
    cmanager->reset(0);
}

Event* NetModel::getEventByNumber(int n)
{
    foreach(Event *event, events)
    {
        if (event->getN()==n)
            return event;
    }
    return NULL;
}

Operation* NetModel::getOperationByEvents(Event* beginEvent, Event* endEvent)
{
    foreach(Operation *operation, operations)
    {
        if (operation->getBeginEvent()==beginEvent && operation->getEndEvent()==endEvent)
            return operation;
    }
    return NULL;
}

bool NetModel::add(Operation* operation)
{
    if (operation)
    {
        if (operations.indexOf(operation)==-1)
        {
            if (operation->getBeginEvent()&&operation->getEndEvent()&&getOperationByEvents(operation->getBeginEvent(), operation->getEndEvent()))
                return false;
            operations << operation;
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool NetModel::remove(Operation* operation)
{
    int index=operations.indexOf(operation);
    if (index!=-1)
    {
        disconnect(operation->getBeginEvent(), operation);
        disconnect(operation, operation->getEndEvent());
        operations.removeAt(index);
        delete operation;
        return true;
    }
    return false;
}

bool NetModel::add(Event* event)
{
    if (event)
    {
        if (event->getN()<0)
            return false;
        foreach (Event *e, events)
            if (e->getN()==event->getN())
                return false;
        if (events.indexOf(event)==-1)
        {
            events << event;
            return true;
        }
        else
           return false;
    }
    else
        return false;
}

bool NetModel::insert(int i, Event* event)
{
    if (event)
    {
        if (event->getN()<0)
            return false;
        foreach (Event *e, events)
            if (e->getN()==event->getN())
                return false;
        if (events.indexOf(event)==-1)
        {
            events.insert(i, event);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

bool NetModel::remove(Event* event/*, bool deleteOutput*/)
{
    int index=events.indexOf(event);
    if (index!=-1)
    {
        QList<Operation*> in = event->getInOperations();
        foreach (Operation *o, in)
            disconnect(o, event);
        event->getInOperations().clear();
        QList<Operation*> out = event->getOutOperations();
        foreach (Operation *o, out)
            disconnect(event, o);
        event->getOutOperations().clear();
        events.removeAt(index);
        delete event;
        return true;
    }
    return false;
}

void NetModel::connect(Event* event, Operation* operation)
{
    if (operation && operation->getBeginEvent()==NULL)
    {
        //add(event);
        //add(operation);
        if (event) event->addOutOperation(operation);
        operation->setBeginEvent(event);
        //cmanager->reset(operation->getBeginEvent());
        //emit updated();
    }
}

void NetModel::connect(Operation* operation, Event* event)
{
    if (operation && operation->getEndEvent()==NULL)
    {
        //add(event);
        //add(operation);
        if (event) event->addInOperation(operation);
        operation->setEndEvent(event);
        //cmanager->reset(operation->getBeginEvent());
        //emit updated();
    }
}

void NetModel::disconnect(Event* event,Operation* operation)
{
    if (event)
    {
        int index=event->getOutOperations().indexOf(operation);
        if (index!=-1)
            event->getOutOperations().removeAt(index);
    }
    if (operation && operation->getBeginEvent()==event)
        operation->setBeginEvent(NULL);
    //cmanager->reset(operation->getBeginEvent());
    //emit updated();
}

void NetModel::disconnect(Operation* operation,Event* event)
{
    if (event)
    {
        int index=event->getInOperations().indexOf(operation);
        if (index!=-1)
            event->getInOperations().removeAt(index);
    }
    if (operation && operation->getEndEvent()==event)
        operation->setEndEvent(NULL);
    //cmanager->reset(operation->getBeginEvent());
    //emit updated();
}

void NetModel::connect(Event *e1, Operation *o, Event *e2)
{
    connect(e1,o);
    connect(o,e2);
}

#define A(i, j) a[i * n + j]

bool NetModel::hasLoops()
{
    int n = events.count();
    bool *a = new bool[n * n];
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            A(i, j) = events[i]->hasEdge(events[j]);
        }
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                A(j, k) = A(j, k) || (A(j, i) && A(i, k));
            }
        }
    }
    for (int i = 0; i < n; ++i)
    {
        if (A(i, i))
        {
            delete[] a;
            return true;
        }
    }

    delete[] a;
    return false;
}

bool NetModel::hasMultiEdges()
{
    foreach (Event *e, events)
    {
        QList<Operation*> operations = e->getOutOperations();
        for (int i=0;i<operations.count();++i)
        {
            int multi=0;
            Event *endEvent=operations[i]->getEndEvent();
            for (int j=i+1;j<operations.count();++j)
            {
                multi += (endEvent==operations[j]->getEndEvent()?1:0);
            }
            if (multi)
                return true;
        }
    }
    return false;
}

bool NetModel::hasOneBeginEvent()
{
    int count=0;
    foreach (Event *event, events)
    {
        if (event->getInOperations().count()==0)
            ++count;
    }
    if (count==1)
        return true;
    else
        return false;
}

bool NetModel::hasOneEndEvent()
{
    int count=0;
    foreach (Event *event, events)
    {
        if (event->getOutOperations().count()==0)
            ++count;
    }
    if (count==1)
        return true;
    else
        return false;
}

bool NetModel::hasUnconnectedEvents()
{
    foreach (Event *e, events)
    {
        if (e->getInOperations().count()==0&&e->getOutOperations().count()==0)
            return true;
    }
    return false;
}

bool NetModel::hasUnconnectedOperations()
{
    foreach (Operation *o, operations)
    {
        if (o->getBeginEvent()==NULL||o->getEndEvent()==NULL)
            return true;
    }
    return false;
}

bool NetModel::isCorrect()
{
    return (!hasLoops() && !hasMultiEdges() && hasOneBeginEvent() && hasOneEndEvent()
            && !hasUnconnectedEvents() && !hasUnconnectedOperations());
}

bool NetModel::isCorrect(QString &s)
{
    s = "";
    bool isCorrect = true;
    if (hasLoops())
    {
        s += "Имеются циклы\n";
        isCorrect = false;
    }
    if (hasMultiEdges())
    {
        s += "Имеются работы с одинаковыми кодами\n";
        isCorrect = false;
    }
    if (!hasOneBeginEvent())
    {
        s += "Исходное событие не определено\n";
        isCorrect = false;
    }
    if (!hasOneEndEvent())
    {
        s += "Завершающее событие не определено\n";
        isCorrect = false;
    }
    if (hasUnconnectedEvents())
    {
        s += "Некоторые события не соединены с работами\n";
        isCorrect = false;
    }
    if (hasUnconnectedOperations())
    {
        s += "Некоторые работы не соединены с событиями\n";
        isCorrect = false;
    }
    if (!isCorrect)
    {
        s += "Сетевая модель некорректна\n";
    }
    return isCorrect;
}

QString NetModel::print()
{
    QString s;
    foreach(Operation* operation, operations)
    {
        s.append("\t"+operation->getCode());
    }
    s += "\n";
    foreach(Event* event, events)
    {
        s.append(QString::number(event->getN()));
        foreach(Operation* operation, operations)
        {
            if (operation->getBeginEvent()==event&&event==operation->getEndEvent())
                s.append("\t2");
            else if (operation->getBeginEvent()==event)
                s.append("\t-1");
            else if (operation->getEndEvent()==event)
                s.append("\t1");
            else
                s.append("\t0");
        }
        s += "\n";
    }
    return s;
}

void NetModel::getPathes(Event *begin, Event *end, QList<Path> *pathes)
{
    if (cmanager->get(begin,end,pathes)) return;
    int ac1=pathes->count();
    foreach(Operation *operation, end->getInOperations())
    {
        if (operation->getBeginEvent()==begin) {
            QList<Event*> events;
            events << begin << end;
            Path path(events);
            pathes->append(path);
        } else {
            Event *prev = operation->getBeginEvent();
            //QList<Path> pathes1;
            int c1 = pathes->count();
            getPathes(begin, prev, pathes);
            int c2 = pathes->count();
            for(int i=c1;i<c2;++i)
            {
                (*pathes)[i].append(end);
            }
        }
    }
    int ac2=pathes->count();
    cmanager->put(begin,end,pathes->mid(ac1,ac2-ac1));
}

QList<Path> *NetModel::getMaxPathes(Event *begin, Event *end)
{
    QList<Path> *pathes = new QList<Path>;
    if (begin!=NULL&&end!=NULL&&begin!=end)
    {
        getPathes(begin,end,pathes);
        double maxweight=0;
        foreach (Path p, *pathes)
        {
            double cur = p.weight();
            if (cur>maxweight)
                maxweight=cur;
        }
        QList<Path> copy = *pathes;
        foreach (Path p, copy)
        {
            if (!qFuzzyCompare(p.weight()+1.0,maxweight+1.0))
            {
                pathes->removeAll(p);
            }
        }
    }
    return pathes;
}

bool pathLessThan(const Path &p1, const Path &p2)
{
    if (p1.events.count()<p2.events.count())
        return true;
    else if (p1.events.count()>p2.events.count())
        return false;
    else
    {
        int i=0;
        while (i<p1.events.count() && p1.events[i]->getN()==p2.events[i]->getN())
        {
            ++i;
        }
        if (i==p1.events.count())
            return false;
        else
            return p1.events[i]->getN()<p2.events[i]->getN();
    }
    //return p1.code() < p2.code();
}

void NetModel::qsort(QList<Path> &pathes)
{
    qSort(pathes.begin(), pathes.end(), pathLessThan);
}

void NetModel::sort(QList<Path> &pathes)
{
    for (int i=0; i<pathes.count()-1; ++i)
    {
        int max = i;
        for (int j=i+1; j<pathes.count(); ++j)
        {
            if (pathes[j].code()<pathes[max].code())
                max = j;
        }
        if (max != i)
        {
            Path tmp = pathes[i];
            pathes.replace(i, pathes[max]);
            pathes.replace(max, tmp);
        }
    }
}

bool eventLessThan(const Event *e1, const Event *e2)
{
    return e1->getN()<e2->getN();
}

bool operationLessThan(const Operation *o1, const Operation *o2)
{
    if (o1->getBeginEvent()->getN()<o2->getBeginEvent()->getN())
        return true;
    else if (o1->getBeginEvent()->getN()>o2->getBeginEvent()->getN())
        return false;
    else if (o1->getEndEvent()->getN()<o2->getEndEvent()->getN())
        return true;
    else
        return false;
//    return o1->getCode()<o2->getCode();
}

QList<Event*> *NetModel::getSortedEvents()
{
    QList<Event*> *list = new QList<Event*>(events);
    /*for (int i=0; i<list->count()-1; ++i)
    {
        int max = i;
        for (int j=i+1; j<list->count(); ++j)
        {
            if (list->at(j)->getN()<list->at(max)->getN())
                max = j;
        }
        if (max!=i)
        {
            Event *tmp = list->at(i);
            list->replace(i, list->at(max));
            list->replace(max, tmp);
        }
    }*/
    qSort(list->begin(), list->end(), eventLessThan);
    return list;
}

QList<Operation*> *NetModel::getSortedOperatioins()
{
    QList<Operation*> *list = new QList<Operation*>(operations);
    /*for (int i=0; i<list->count()-1; ++i)
    {
        int max = i;
        for (int j=i+1; j<list->count(); ++j)
        {
            if (list->at(j)->getCode()<list->at(max)->getCode())
                max = j;
        }
        if (max!=i)
        {
            Operation *tmp = list->at(i);
            list->replace(i, list->at(max));
            list->replace(max, tmp);
        }
    }*/
    qSort(list->begin(), list->end(), operationLessThan);
    return list;
}

double NetModel::getMaxPathWeight(Event *begin, Event *end)
{
    double w = 0;
    QList<Path> *pathes = getMaxPathes(begin, end);
    if (pathes->count()>0)
        w = pathes->first().weight();
    delete pathes;
    return w;
}

void NetModel::getBeginEndEvents(Event**begin,Event**end)
{
    foreach (Event *e, events)
    {
        if (e->getInOperations().count()==0)
            *begin = e;
        else if (e->getOutOperations().count()==0)
            *end = e;
    }
}

Event *NetModel::getBeginEvent()
{
    Event *begin=NULL;
    foreach (Event *e, events)
    {
        if (e->getInOperations().count()==0)
            begin = e;
    }
    return begin;
}

Event *NetModel::getEndEvent()
{
    Event *end=NULL;
    foreach (Event *e, events)
    {
        if (e->getOutOperations().count()==0)
            end = e;
    }
    return end;
}

QList<Path> *NetModel::_getCriticalPathes()
{
    Event *begin, *end;
    getBeginEndEvents(&begin,&end);
    return getMaxPathes(begin,end);
}

QList<Path> *NetModel::_getFullPathes()
{
    Event *begin, *end;
    getBeginEndEvents(&begin,&end);
    QList<Path> *pathes = new QList<Path>();
    getPathes(begin,end,pathes);
    return pathes;
}

QList<Path> *NetModel::getCriticalPathes()
{
    if (!criticPathes)
        criticPathes = _getCriticalPathes();
    return criticPathes;//new QList<Path>(*criticPathes);
//    return _getCriticalPathes();
}

QList<Path> *NetModel::getFullPathes()
{
    if (!fullPathes)
    {
        fullPathes = _getFullPathes();
        qsort(*fullPathes);
    }
    return fullPathes;//new QList<Path>(*fullPathes);
//    return _getFullPathes();
}

double NetModel::getCriticalPathWeight()
{
    double w = 0;
    QList<Path> *pathes = getCriticalPathes();
    if (pathes->count()>0)
        w = pathes->first().weight();
    //delete pathes;
    return w;
}

double NetModel::getEarlyEndTime(Event *i)
{
    return getMaxPathWeight(getBeginEvent(), i);
}

double NetModel::getLaterEndTime(Event *i)
{
    return getCriticalPathWeight()-getMaxPathWeight(i, getEndEvent());
}

double NetModel::getEarlyStartTime(Operation *o)
{
    return getEarlyEndTime(o->getBeginEvent());
}

double NetModel::getLaterStartTime(Operation *o)
{
    return getLaterEndTime(o->getEndEvent())-o->getWaitTime();
}

double NetModel::getEarlyEndTime(Operation *o)
{
    return getEarlyEndTime(o->getBeginEvent())+o->getWaitTime();
}

double NetModel::getLaterEndTime(Operation *o)
{
    return getLaterEndTime(o->getEndEvent());
}

double NetModel::getReserveTime(const Path &p)
{
    return getCriticalPathWeight()-p.weight();
}

double NetModel::getReserveTime(Event *e)
{
    return getLaterEndTime(e)-getEarlyEndTime(e);
}

double NetModel::getFullReserveTime(Operation *o)
{
    return getLaterEndTime(o)-getEarlyEndTime(o);
    //return getLaterStartTime(o)-getEarlyStartTime(o); // it's the same
}

double NetModel::getFreeReserveTime(Operation *o)
{
    return getEarlyEndTime(o->getEndEvent())-getEarlyEndTime(o->getBeginEvent())-o->getWaitTime();
}

bool NetModel::setN(Event *e, int n)
{
    if (n<0)
        return false;
    foreach (Event *event, events)
    {
        if (event->getN()==n)
            return false;
    }
    e->setN(n);
    emit eventIdChanged(e, n);
    emit updated();
    return true;
}

bool NetModel::setName(Event *e, const QString &name)
{
    e->setName(name);
    emit eventNameChanged(e, name);
    emit updated();
    return true;
}

bool NetModel::setOperationEndEvent(Operation *o, Event *e)
{
    //if (!e) return false;
    if (getOperationByEvents(o->getBeginEvent(), e))
        return false;
    else
    {
        Event *old = o->getEndEvent();
        disconnect(o, old);
        connect(o, e);
        emit operationEndEventChanged(o, old);
        emit updated();
        return true;
    }
}

bool NetModel::setOperationName(Operation *o, const QString &name)
{
    o->setName(name);
    emit operationNameChanged(o, name);
    emit updated();
    return true;
}

bool NetModel::setOperationWaitTime(Operation *o, double twait)
{
    if (twait>=0)
    {
        o->setWaitTime(twait);
        cmanager->reset(o->getBeginEvent());
        emit operationWaitTimeChanged(o, twait);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::addEvent()
{
    Event *e = new Event(generateId());
    if (add(e))
    {
        emit afterEventAdd();
        emit updated();
        return true;
    }
    else
    {
        delete e;
        return false;
    }
}

bool NetModel::insertEvent(int i)
{
    Event *e = new Event(generateId());
    if (insert(i, e))
    {
        emit afterEventInsert(i);
        emit updated();
        return true;
    }
    else
    {
        delete e;
        return false;
    }
}

bool NetModel::removeEvent(Event *e)
{
    emit beforeEventDelete(e);
    if (remove(e))
    {
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::addOperation(Operation *o)
{
    if (add(o))
    {
        emit afterOperationAdd(o);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::insertOperation(Operation *o, int i)
{
    if (add(o))
    {
        if (o->beginEvent)
            o->beginEvent->insertOutOperation(o, i);
        emit afterOperationInsert(o, i);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::removeOperation(Operation *o)
{
    emit beforeOperationDelete(o);
    if (remove(o))
    {
        emit updated();
        return true;
    }
    else
        return false;
}

int NetModel::generateId()
{
    QSet<int> set;
    foreach (Event *e, events)
        set += e->getN();
    for (int i = 0; i <= std::numeric_limits<int>::max(); ++i)
    {
        if (!set.contains(i))
            return i;
    }
    return std::numeric_limits<int>::max();
}

QDataStream &NetModel::writeEvent(Event *e, QDataStream &stream)
{
    stream << e->getN() << e->getName() << e->getPoint();
    return stream;
}

QDataStream &NetModel::readEvent(Event **e, QDataStream &stream)
{
    int n;
    QString name;
    QPoint point;
    stream >> n >> name >> point;
    if (stream.status()==QDataStream::Ok)
    {
        *e = new Event(n);
        (*e)->setName(name);
        (*e)->getPoint()=point;
    }
    else
        *e = NULL;
    return stream;
}

QDataStream &NetModel::writeOperation(Operation *o, QDataStream &stream)
{
    if (o->getBeginEvent())
        stream << o->getBeginEvent()->getN();
    else
        stream << -1;
    if (o->getEndEvent())
        stream << o->getEndEvent()->getN();
    else
        stream << -1;
    stream << o->getWaitTime() << o->getName();
    return stream;
}

QDataStream &NetModel::readOperation(Operation **o, QDataStream &stream)
{
    int begin, end;
    double twait;
    QString name;
    stream >> begin >> end >> twait >> name;
    if (stream.status()==QDataStream::Ok)
    {
        *o = new Operation();
        if (begin==-1)
            connect(NULL, *o);
        else
            connect(getEventByNumber(begin), *o);
        if (end==-1)
            connect(*o, NULL);
        else
            connect(*o, getEventByNumber(end));
        (*o)->setName(name); // setOperationName(this, *o, name);
        (*o)->setWaitTime(twait); // setOperationWaitTime(this, *o, twait);
    }
    else
        *o = NULL;
    return stream;
}

QDataStream &NetModel::writeTo(QDataStream &stream)
{
    stream << events.count();
    stream << operations.count();
    foreach (Event *e, events)
    {
        writeEvent(e, stream);
    }
    foreach (Operation *o, operations)
    {
        writeOperation(o, stream);
    }
    return stream;
}

QDataStream &NetModel::readFrom(QDataStream &stream)
{
    int eventscount, operationscount;
    stream >> eventscount >> operationscount;
    if (stream.status()==QDataStream::Ok)
    {
        for (int i = 0; i < eventscount; ++i)
        {
            Event *e;
            readEvent(&e, stream);
            add(e);
        }
        for (int i = 0; i < operationscount; ++i)
        {
            Operation *o;
            readOperation(&o, stream);
            add(o);
        }
        updateCriticalPath();
    }
    return stream;
}

void NetModel::clear()
{
    emit beforeClear();
    qDeleteAll(events);
    events.clear();
    qDeleteAll(operations);
    operations.clear();
}

bool NetModel::inCriticalPath(Operation *o)
{
    if (isCorrect())
    {
        QList<Path> *pathes = getCriticalPathes();
        bool contains = false;
        foreach (Path path, *pathes)
        {
            if (path.contains(o))
            {
                contains = true;
                break;
            }
        }
        //delete pathes;
        return contains;
    }
    else
        return false;
}

double NetModel::getIntensityFactor(Operation *)
{
    return -1;
}
