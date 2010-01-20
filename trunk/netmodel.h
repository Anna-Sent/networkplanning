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
public:
    Operation();
    Operation(double);
    Event *getBeginEvent();
    Event *getEndEvent();
    void setBeginEvent(Event* e);
    void setEndEvent(Event* e);
    double getWaitTime();
    void setWaitTime(double twait) {this->twait=twait;}
    QString getCode();
    QString getName() {return name;}
    void setName(const QString &name) {this->name=name;}
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
    QList<Path> *getMaxPathes(Event*,Event*);
    double getMaxPathWeight(Event *, Event *);
    void getPathes(Event*,Event*,QList<Path>*);
    bool add(Operation*);
    bool remove(Operation*);
    bool add(Event*);
    bool insert(int, Operation*);
    bool insert(int, Event*);
    bool remove(Event*/*, bool deleteOutput = true*/);
    QDataStream &writeEvent(Event *e, QDataStream &stream);
    QDataStream &readEvent(Event **e, QDataStream &stream);
    QDataStream &writeOperation(Operation *o, QDataStream &stream);
    QDataStream &readOperation(Operation **o, QDataStream &stream);
public:
    NetModel();
    ~NetModel();
    // utils
    int generateId();
    Event* getEventByNumber(int n);
    Operation* getOperationByEvents(Event*,Event*);
    QString print();
    void getBeginEndEvents(Event**,Event**);
    Event* getBeginEvent();
    Event* getEndEvent();
    QDataStream &writeTo(QDataStream &stream);
    QDataStream &readFrom(QDataStream &stream);
    void clear();
    // graph editors
    void connect(Event*,Operation*);
    void connect(Operation*,Event*);
    void connect(Event*,Operation*,Event*);
    void disconnect(Event*,Operation*);
    void disconnect(Operation*,Event*);
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
    QList<Event*> *getEvents() {return &events;}
    int getEventsCount() {return events.count();}
    Event *event(int i) {return i>=0&&i<events.count()?events[i]:NULL;}
    Event *first() {return events.isEmpty()?NULL:events.first();}
    Event *last() {return events.isEmpty()?NULL:events.last();}
    QList<Operation*> *getOperations() {return &operations;}
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
public slots:
    bool setN(QObject *, Event *, int);
    bool setName(QObject *, Event *, const QString &);
    bool setOperationEndEvent(QObject *, Operation **, Event *);
    bool setOperationName(QObject *, Operation *, const QString &);
    bool setOperationWaitTime(QObject *, Operation *, double);
    bool addEvent(QObject */*, Event */);
    bool removeEvent(QObject *, Event *);
    bool addOperation(QObject *, Operation *);
    bool removeOperation(QObject *, Operation */*, bool deleteOutput = true*/);
    bool insertEvent(QObject *, /*Event *,*/ int);
    bool insertOperation(QObject *, Operation *, int);
signals:
    void eventIdChanged(QObject *, Event *, int);
    void eventNameChanged(QObject *, Event *, const QString &);
    void operationEndEventChanged(QObject *, Operation **, Event *);
    void operationNameChanged(QObject *, Operation *, const QString &);
    void operationWaitTimeChanged(QObject *, Operation *, double);
    void afterEventAdd(QObject */*, Event */);
    void beforeEventDelete(QObject *, Event *);
    void afterOperationAdd(QObject *, Operation *);
    void beforeOperationDelete(QObject *, Operation */*, bool deleteOutput = true*/);
    void afterEventInsert(QObject *,/* Event *,*/ int);
    void afterOperationInsert(QObject *, Operation *, int);
    void updated();
    void beforeClear();
};

#endif // NETMODEL_H














