#ifndef PLOTFRAME_H
#define PLOTFRAME_H

#include <QGraphicsItemGroup>
#include <QList>
#include <QVector>
#include <QString>
#include <QGraphicsSimpleTextItem>

class PlotFrame : public QGraphicsItemGroup
{
  qreal vertical_offset;

  QList<QGraphicsSimpleTextItem *> callers;
  QVector<QGraphicsLineItem *> lines;

public:
  PlotFrame(qreal offset = 50, QGraphicsItem * parent = 0);

  void addCaller(const QString &caller);
  void setWidth(qreal width);
};

#endif // PLOTFRAME_H
