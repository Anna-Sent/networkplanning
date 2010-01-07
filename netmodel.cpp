#include "netmodel.h"
#include <QDebug>

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

QString Path::print() const
{
    QString s;
    foreach(Event*event,events)
    {
        s += QString::number(event->getN()) + " ";
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
        if (operation->getBeginEvent())
        {
            int index1 = operation->getBeginEvent()->getOutOperations().indexOf(operation);
            if (index1!=-1)
                operation->getBeginEvent()->getOutOperations().removeAt(index1);
        }
        if (operation->getEndEvent())
        {
            int index1 = operation->getEndEvent()->getInOperations().indexOf(operation);
            if (index1!=-1)
                operation->getEndEvent()->getInOperations().removeAt(index1);
        }
        operations.removeAt(index);
        delete operation;
        return true;
    }
    return false;
}

bool NetModel::add(Event* event)
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

bool NetModel::insert(int i, Event* event)
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

bool NetModel::remove(Event* event/*, bool deleteOutput*/)
{
    int index=events.indexOf(event);
    if (index!=-1)
    {
        foreach (Operation *o, event->getInOperations())
            o->setEndEvent(NULL);
        /*if (deleteOutput)
        {*/
            foreach (Operation *o, event->getOutOperations())
                operations.removeAt(operations.indexOf(o));
            for (int i=0; i < event->getOutOperations().count(); ++i)
            {
                Operation *o = event->getOutOperations()[i];
                o->getEndEvent()->getInOperations().removeAt(
                        o->getEndEvent()->getInOperations().indexOf(o));
                delete o;
            }
            event->getOutOperations().clear();
        /*}
        else
        {
            foreach (Operation *o, event->getOutOperations())
                o->setBeginEvent(NULL);
        /*}*/
        events.removeAt(index);
        delete event;
        return true;
    }
    return false;
}

void NetModel::connect(Event* event,Operation* operation)
{
    if (operation->getBeginEvent()==NULL)
    {
        add(event);
        add(operation);
        event->addOutOperation(operation);
        operation->setBeginEvent(event);
    }
}

void NetModel::connect(Operation* operation,Event* event)
{
    if (operation->getEndEvent()==NULL)
    {
        add(event);
        add(operation);
        event->addInOperation(operation);
        operation->setEndEvent(event);
    }
}

void NetModel::disconnect(Event* event,Operation* operation)
{
    int index=event->getOutOperations().indexOf(operation);
    qDebug()<<event->getOutOperations().count();
    if (index!=-1)
    {
        event->getOutOperations().removeAt(index);
        operation->setBeginEvent(NULL);
        qDebug()<<event->getOutOperations().count();
    }
}

void NetModel::disconnect(Operation* operation,Event* event)
{
    int index=event->getInOperations().indexOf(operation);
    if (index!=-1)
    {
        event->getInOperations().removeAt(index);
        operation->setEndEvent(NULL);
    }
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

QList<Path> *NetModel::getMaxPathes(Event *begin, Event *end, double *maxweight)
{
    QList<Path> *pathes = new QList<Path>;
    if (begin!=NULL&&end!=NULL&&begin!=end)
    {
        getPathes(begin,end,pathes);
        *maxweight=0;
        for (int i=0;i<pathes->count();++i)
        {
            double cur = (*pathes)[i].weight();
            if (cur-*maxweight>1e-9)
                *maxweight=cur;
        }
        for (int i=0;i<pathes->count();++i)
        {
            if (abs((*pathes)[i].weight()-*maxweight)>1e-9)
                pathes->removeAt(i);
        }
    }
    return pathes;
}

QList<Path> *NetModel::getMaxPathes(Event *begin, Event *end)
{
    double w;
    return getMaxPathes(begin,end,&w);
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

QList<Path> *NetModel::getCriticalPath()
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
    Event *begin, *end;
    getBeginEndEvents(&begin,&end);
    double w;
    getMaxPathes(begin,end,&w);
    return w;
}

double NetModel::getEarlyEndTime(Event *i)
{
    Event *J = getBeginEvent();
    double w;
    getMaxPathes(J,i,&w);
    return w;
}

double NetModel::getLaterEndTime(Event *i)
{
    double w, tcr = getCriticalPathWeight();
    getMaxPathes(i,getEndEvent(),&w);
    return tcr-w;
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

double NetModel::getReserveTime(Path &p)
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
    //return getLaterStartTime(o)-getEarlyStartTime(o);
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
    return true;
}

bool NetModel::setName(QObject *obj, Event *e, const QString &name)
{
    e->setName(name);
    emit eventNameChanged(obj, e, name);
    return true;
}

bool NetModel::setOperationEndEvent(QObject *obj, Operation **o, Event *e)
{
    Operation *o1 = getOperationByEvents((*o)->getBeginEvent(), e);
    if (o1)
    {
        /*Event *begin = (*o)->getBeginEvent();
        Event *end = (*o)->getEndEvent();
        if (begin)
            begin->getOutOperations().replace(begin->getOutOperations().indexOf(*o), o1);
        if (end)
            end->getInOperations().replace(end->getInOperations().indexOf(*o), o1);
        operations.replace(operations.indexOf(*o), o1);*/
        delete *o;
        *o = o1;
        return false;
    }
    else
    {
        Event *end = (*o)->getEndEvent();
        if (end)
            end->getInOperations().removeAt(end->getInOperations().indexOf(*o));
        (*o)->setEndEvent(e);
        e->addInOperation(*o);
    }
    emit operationEndEventChanged(obj, o, e);
    return true;
}

bool NetModel::setOperationName(QObject *obj, Operation *o, const QString &name)
{
    o->setName(name);
    emit operationNameChanged(obj, o, name);
    return true;
}

bool NetModel::setOperationWaitTime(QObject *obj, Operation *o, double twait)
{
    o->setWaitTime(twait);
    emit operationWaitTimeChanged(obj, o, twait);
    return true;
}

bool NetModel::addEvent(QObject *obj, Event *e)
{
    if (add(e)) {
        emit afterEventAdd(obj, e);
        return true;
    }
    else
        return false;
}

bool NetModel::insertEvent(QObject *obj, Event *e, int i)
{
    if (insert(i, e)) {
        emit afterEventInsert(obj, e, i);
        return true;
    }
    else
        return false;
}

bool NetModel::removeEvent(QObject *obj, Event *e)
{
    emit beforeEventDelete(obj, e);
    if (remove(e))
        return true;
    else
        return false;
}

bool NetModel::addOperation(QObject *view, Operation *o)
{
    if (add(o)) {
        emit afterOperationAdd(view, o);
        return true;
    }
    else
        return false;
}

bool NetModel::insertOperation(QObject *view, Operation *o, int i)
{
    if (insert(i, o)) {
        emit afterOperationInsert(view, o, i);
        return true;
    }
    else
        return false;
}

bool NetModel::removeOperation(QObject *view, Operation *o)
{
    emit beforeOperationDelete(view, o);
    if (remove(o))
        return true;
    else
        return false;
}
