#include "netmodel.h"
#include <QDebug>
#include <limits>

using namespace std;

Event::Event()
{
    n = 0;
}

Event::Event(int n)
{
    this->n = n;
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

int Event::getN()
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
        beginEvent(NULL), endEvent(NULL), tmin(0), tmax(0), twait(0)
{
}

Operation::Operation(double twait) :
        beginEvent(NULL), endEvent(NULL), tmin(0), tmax(0), twait(twait)
{
}

Event *Operation::getBeginEvent()
{
    return beginEvent;
}

Event *Operation::getEndEvent()
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

QString Operation::getCode()
{
    QString first=beginEvent?QString::number(beginEvent->getN()):"NULL";
    QString second=endEvent?QString::number(endEvent->getN()):"NULL";
    return first+"-"+second;
}

Path::Path(QList<Event*> events)
{
    this->events = events;
}

QString Path::code() const
{
    QString s;
    int count = events.count();
    if (count>0)
    {
        for (int i=0;i<count-1;++i)
        {
            Event *event = events[i];
            s += QString::number(event->getN()) + "-";
        }
        s += QString::number(events.last()->getN());
    }
    return s;
}

double Path::weight() const
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

NetModel::NetModel()
{
}

NetModel::~NetModel()
{
    qDeleteAll(events);
    events.clear();
    qDeleteAll(operations);
    operations.clear();
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
            if (operation->getBeginEvent() && operation->getEndEvent())
            {
                Operation *o= getOperationByEvents(operation->getBeginEvent(), operation->getEndEvent());
                if (o)
                {
                    delete operation;
                    operation = o;
                }
            }
            operations << operation;
            return true;
        }
        else
            return false;
    }
else
    return false;
}

bool NetModel::insert(int i, Operation* operation)
{
    if (operations.indexOf(operation)==-1)
    {
        if (operation->getBeginEvent() && operation->getEndEvent())
        {
            Operation *o= getOperationByEvents(operation->getBeginEvent(), operation->getEndEvent());
            if (o)
            {
                delete operation;
                operation = o;
            }
        }
        operations.insert(i, operation);
        return true;
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
    bool deleteOutput = true;
    int index=events.indexOf(event);
    if (index!=-1)
    {
        QList<Operation*> in = event->getInOperations();
        foreach (Operation *o, in)
            disconnect(o, event);
        event->getInOperations().clear();
        QList<Operation*> out = event->getOutOperations();
        if (deleteOutput)
            foreach (Operation *o, out)
                remove(o);
        else
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
        add(event);
        add(operation);
        if (event) event->addOutOperation(operation);
        operation->setBeginEvent(event);
    }
}

void NetModel::connect(Operation* operation, Event* event)
{
    if (operation && operation->getEndEvent()==NULL)
    {
        add(event);
        add(operation);
        if (event) event->addInOperation(operation);
        operation->setEndEvent(event);
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
}

void NetModel::connect(Event *e1, Operation *o, Event *e2)
{
    connect(e1,o);
    connect(o,e2);
}

bool NetModel::hasLoops()
{
    int n=events.count();
    bool a[n][n];
    for (int i=0; i<n; ++i)
        for (int j=0; j<n; ++j)
            a[i][j] = events[i]->hasEdge(events[j]);
    for (int i=0;i<n;++i)
        for (int j=0;j<n;++j)
            for (int k=0;k<n;++k)
                a[j][k] = a[j][k] || (a[j][i] && a[i][k]);
    for (int i=0; i<n; ++i)
            if (a[i][i])
                return true;
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
    foreach(Operation *operation, begin->getOutOperations())
    {
        if (operation->getEndEvent()==end) {
            QList<Event*> events;
            events << begin << end;
            Path path(events);
            pathes->append(path);
        } else {
            Event *next = operation->getEndEvent();
            QList<Path> pathes1;
            getPathes(next, end, &pathes1);
            foreach(Path path1, pathes1)
            {
                QList<Event*> events;
                events << begin;
                events << path1.events;
                Path path(events);
                pathes->append(path);
            }
        }
    }
}

QList<Path> *NetModel::getMaxPathes(Event *begin, Event *end)
{
    QList<Path> *pathes = new QList<Path>;
    if (begin!=NULL&&end!=NULL&&begin!=end)
    {
        getPathes(begin,end,pathes);
        double maxweight=0;
        for (int i=0;i<pathes->count();++i)
        {
            double cur = (*pathes)[i].weight();
            if (cur-maxweight>1e-9)
                maxweight=cur;
        }
        for (int i=0;i<pathes->count();++i)
        {
            if (abs((*pathes)[i].weight()-maxweight)>1e-9)
                pathes->removeAt(i);
        }
    }
    return pathes;
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

QList<Path> *NetModel::getCriticalPathes()
{
    Event *begin, *end;
    getBeginEndEvents(&begin,&end);
    return getMaxPathes(begin,end);
}

QList<Path> *NetModel::getFullPathes()
{
    Event *begin, *end;
    getBeginEndEvents(&begin,&end);
    QList<Path> *pathes = new QList<Path>();
    getPathes(begin,end,pathes);
    return pathes;
}

double NetModel::getCriticalPathWeight()
{
    double w = 0;
    QList<Path> *pathes = getCriticalPathes();
    if (pathes->count()>0)
        w = pathes->first().weight();
    delete pathes;
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

bool NetModel::setN(QObject *obj, Event *e, int n)
{
    foreach (Event *event, events)
    {
        if (event->getN()==n)
            return false;
    }
    e->setN(n);
    emit eventIdChanged(obj, e, n);
    emit updated();
    return true;
}

bool NetModel::setName(QObject *obj, Event *e, const QString &name)
{
    e->setName(name);
    emit eventNameChanged(obj, e, name);
    emit updated();
    return true;
}

bool NetModel::setOperationEndEvent(QObject *obj, Operation **o, Event *e)
{
    //if (!e) return false;
    Operation *o1 = getOperationByEvents((*o)->getBeginEvent(), e);
    if (o1)
    {
        return false;
    }
    else
    {
        disconnect(*o, (*o)->getEndEvent());
        connect(*o, e);
    }
    emit operationEndEventChanged(obj, o, e);
    emit updated();
    return true;
}

bool NetModel::setOperationName(QObject *obj, Operation *o, const QString &name)
{
    o->setName(name);
    emit operationNameChanged(obj, o, name);
    emit updated();
    return true;
}

bool NetModel::setOperationWaitTime(QObject *obj, Operation *o, double twait)
{
    o->setWaitTime(twait);
    emit operationWaitTimeChanged(obj, o, twait);
    emit updated();
    return true;
}

bool NetModel::addEvent(QObject *obj, Event *e)
{
    if (add(e))
    {
        emit afterEventAdd(obj, e);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::insertEvent(QObject *obj, Event *e, int i)
{
    if (insert(i, e))
    {
        emit afterEventInsert(obj, e, i);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::removeEvent(QObject *obj, Event *e)
{
    emit beforeEventDelete(obj, e);
    if (remove(e))
    {
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::addOperation(QObject *view, Operation *o)
{
    if (add(o))
    {
        emit afterOperationAdd(view, o);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::insertOperation(QObject *view, Operation *o, int i)
{
    if (insert(i, o))
    {
        emit afterOperationInsert(view, o, i);
        emit updated();
        return true;
    }
    else
        return false;
}

bool NetModel::removeOperation(QObject *view, Operation *o)
{
    emit beforeOperationDelete(view, o);
    if (remove(o))
    {
        return true;
        emit updated();
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















