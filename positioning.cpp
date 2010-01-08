#include "positioning.h"
#include "eventwidget.h"
#include <QDebug>

void PlanarPosition::position(NetModel* model)
{
	typedef QSet<Event*> evlist;
	Event* begin = model->getBeginEvent();

	evlist wave;
	evlist nextWave;
	wave.insert(begin);
	int x=10;
	int y=10;
	begin->getPoint()=QPoint(x,y);
					EventWidget *ic = new EventWidget(begin,model,_widget);
					ic->setText(QString::number(begin->getN()));
					ic->move(x, y);
					ic->show();
					ic->setAttribute(Qt::WA_DeleteOnClose);
x+=30;
	while(!wave.isEmpty()) {
		nextWave.clear();
		y=10;
		foreach(Event* cev,wave)
		{
			QList<Operation*> ops = cev->getOutOperations();
			qDebug() << ops.count();
			foreach(Operation *op,ops)
			{
				Event *end=op->getEndEvent();
				if (end) {
					end->getPoint()=QPoint(x,y);
					EventWidget *ic = new EventWidget(end,model,_widget);
					ic->setText(QString::number(end->getN()));
					ic->move(x, y);
					ic->show();
					ic->setAttribute(Qt::WA_DeleteOnClose);

					nextWave.insert(end);
					y+=30;
				}
			}
		}
		x+=30;
		wave=nextWave;
	}
}

