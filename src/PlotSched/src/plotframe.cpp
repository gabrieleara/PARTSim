#include "plotframe.h"

PlotFrame::PlotFrame(qreal offset, QGraphicsItem *parent) :
  QGraphicsItemGroup(parent)
{
  vertical_offset = offset;
}

/// adds a row to the plot
void PlotFrame::addRow(const QString &title)
{
    unsigned int count = callers.count();

    qreal y = count * vertical_offset;

    QGraphicsSimpleTextItem * t = new QGraphicsSimpleTextItem(title, this);
    t->setPos(-t->boundingRect().width() - 10, y - t->boundingRect().height());
    callers.append(t);
    this->addToGroup(t);

    QGraphicsLineItem * l = new QGraphicsLineItem(0, y, 0, y, this);
    lines.append(l);
    this->addToGroup(l);
}

void PlotFrame::setWidth(qreal width)
{
  for (QVector<QGraphicsLineItem *>::iterator it = lines.begin(); it != lines.end(); ++it) {
    QLineF old_line = (*it)->line();
    (*it)->setLine(0, old_line.y1(), width, old_line.y1());
  }
}
