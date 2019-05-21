#include "eventsmanager.h"

#include <QDebug>

EventsManager::EventsManager()
{
  last_event = 0;
  last_magnification = 1;
}


void EventsManager::clear()
{
  events_container.clear();
  last_magnification = 1;
  last_event = 0;
}


void EventsManager::newEventArrived(Event e)
{
  QList<Event>::iterator i = events_container[e.getCaller()].begin();
  while (i != events_container[e.getCaller()].end() && (*i).getStart() < e.getStart())
    ++i;
  events_container[e.getCaller()].insert(i, e);
  if (e.getStart() > last_event)
    last_event = e.getStart();
}


unsigned long EventsManager::countCallers()
{
  return events_container.count();
}


QList<Event> * EventsManager::getCallerEventsList(unsigned long caller)
{
  unsigned long c = 0;
  QMap <QString, QList<Event>>::iterator i = events_container.begin();
  while (c <= caller && i != events_container.end()) {
    ++i;
    ++c;
  }
  return &(*i);
}


QMap <QString, QList<Event>> * EventsManager::getCallers()
{
  return &events_container;
}


qreal EventsManager::magnify(qreal start, qreal end, qreal width)
{
  qreal new_center;
  qreal fraction;
  qreal size = end - start;
  qreal magnification = width / size;

  new_center = (start + end) / 2 / last_magnification;

  if (size > 0)
    fraction = magnification;
  else
    fraction = -last_magnification / magnification;

  last_magnification = fraction;

  for (QMap<QString, QList<Event>>::iterator l = events_container.begin();
       l != events_container.end();
       ++l) {
    for (QList<Event>::iterator i = (*l).begin(); i != (*l).end(); ++i)
      (*i).setMagnification(fraction);
  }

  qDebug() << "The magnification is : " << fraction;
  if (size > 0)
    qDebug() << "The new margins are : " << start * fraction << " " << end * fraction;

  new_center *= last_magnification;

  return new_center;
}
