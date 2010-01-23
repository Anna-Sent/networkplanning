#ifndef NETMODEL_H
#define NETMODEL_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDebug>
#include <QMetaType>
#include <QPoint>
#include <QDataStream>

class Operation;
class NetModel;

class Event
{
private:
    QList<Operation*> inputOperations, outputOperations;
    int n;
    QString name;
    QPoint point;
    Event();
    Event(int);
    void setN(int n) {this->n=n;}
    void addInOperation(Operation*);
    void addOutOperation(Operation*);
    void insertInOperation(Operation *o, int i) {inputOperations.insert(i, o);}
    bool hasEdge(Event*);
    void setName(const QString &name) {this->name=name;}
    friend class NetModel;
public:
    void insertOutOperation(Operation *o, int i) {outputOperations.insert(i, o);}
    QPoint& getPoint() {return point;}
    int getN();
    QList<Operation*>& getInOperations();
    QList<Operation*>& getOutOperations();
    QString getName() {return name;}
};

Q_DECLARE_METATYPE(Event*);
Q_DECLARE_METATYPE(Operation*);

class Operation
{
private:
    Event *beginEvent, *endEvent;
    double tmin, tmax, twait;
    QString name;
    void setWaitTime(double twait) {this->twait=twait;}
    void setName(const QString &name) {this->name=name;}
    friend class NetModel;
public:
    Operation();
    Operation(double);
    Event *getBeginEvent();
    Event *getEndEvent();
    void setBeginEvent(Event* e);
    void setEndEvent(Event* e);
    double getWaitTime();
    QString getCode();
    QString getName() {return name;}
};

class Path
{
private:
    double _weight;
    QString _code;
    double calcWeight() const;
    QString calcCode() const;
public:
    QList<Event*> events;
    Path(QList<Event*> events);
    QString code() const {return _code;}
    double weight() const {return _weight;}
    bool operator==(const Path &p)
    {
        if (events.count()==p.events.count())
        {
            for (int i=0;i<events.count();++i)
            {
                if (events[i]!=p.events[i])
                    return false;
            }
        }
        else
        {
            return false;
        }
        return true;
    }
};

class NetModel : public QObject
{
    Q_OBJECT
private:
    QList<Event*> events;
    QList<Operation*> operations;
    QList<Path> *getMaxPathes(Event *, Event *);
    double getMaxPathWeight(Event *, Event *);
    void getPathes(Event *, Event *, QList<Path> *);
    bool add(Operation *);
    bool remove(Operation *);
    bool add(Event *);
    bool insert(int, Event *);
    bool remove(Event *);
    QDataStream &writeEvent(Event *e, QDataStream &stream);
    QDataStream &readEvent(Event **e, QDataStream &stream);
    QDataStream &writeOperation(Operation *o, QDataStream &stream);
    QDataStream &readOperation(Operation **o, QDataStream &stream);
    int generateId();
public:
    NetModel();
    ~NetModel();
    Event* getEventByNumber(int n);
    Operation* getOperationByEvents(Event *, Event *);
    void getBeginEndEvents(Event **, Event **);
    Event *getBeginEvent();
    Event *getEndEvent();
    QString print();
    QDataStream &writeTo(QDataStream &stream);
    QDataStream &readFrom(QDataStream &stream);
    // checkers
    bool hasLoops();
    bool hasMultiEdges();
    bool hasOneBeginEvent();
    bool hasOneEndEvent();
    bool hasUnconnectedEvents();
    bool hasUnconnectedOperations();
    bool isCorrect();
    bool isCorrect(QString &);
    // getters
    QList<Operation*> *getOperations() {return &operations;}
    QList<Event*> *getEvents() {return &events;}
    int getEventsCount() {return events.count();}
    Event *event(int i) {return i>=0&&i<events.count()?events[i]:NULL;}
    Event *first() {return events.isEmpty()?NULL:events.first();}
    Event *last() {return events.isEmpty()?NULL:events.last();}
    // for net
    QList<Path> *getFullPathes();
    QList<Path> *getCriticalPathes();
    double getCriticalPathWeight();
    double getEarlyEndTime(Event*);
    double getLaterEndTime(Event*);
    double getEarlyStartTime(Operation*);
    double getLaterStartTime(Operation*);
    double getEarlyEndTime(Operation*);
    double getLaterEndTime(Operation*);
    double getReserveTime(const Path&);
    double getReserveTime(Event*);
    double getFullReserveTime(Operation*);
    double getFreeReserveTime(Operation*);
public:
    void connect(Event*,Operation*);
    void connect(Operation*,Event*);
    void connect(Event*,Operation*,Event*);
    void disconnect(Event*,Operation*);
    void disconnect(Operation*,Event*);
public slots:
    void clear();
    bool setN(Event *, int);
    bool setName(Event *, const QString &);
    bool setOperationEndEvent(Operation **, Event *);
    bool setOperationName(Operation *, const QString &);
    bool setOperationWaitTime(Operation *, double);
    bool addEvent();
    bool removeEvent(Event *);
    bool addOperation(Operation *);
    bool removeOperation(Operation *);
    bool insertEvent(int);
    bool insertOperation(Operation *, int);
signals:
    void beforeClear();
    void eventIdChanged(Event *, int);
    void eventNameChanged(Event *, const QString &);
    void operationEndEventChanged(Operation **, Event *);
    void operationNameChanged(Operation *, const QString &);
    void operationWaitTimeChanged(Operation *, double);
    void afterEventAdd();
    void beforeEventDelete(Event *);
    void afterOperationAdd(Operation *);
    void beforeOperationDelete(Operation *);
    void afterEventInsert(int);
    void afterOperationInsert(Operation *, int);
    void updated();
};

#endif // NETMODEL_H














