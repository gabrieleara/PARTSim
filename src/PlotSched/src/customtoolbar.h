#ifndef CUSTOMTOOLBAR_H
#define CUSTOMTOOLBAR_H

#include <QObject>
#include <QToolBar>

class CustomToolBar : public QToolBar
{
  Q_OBJECT

public:
  CustomToolBar(QWidget *parent = 0);

public slots:
  void buttonOpenSlot();
  void buttonRefreshSlot();

  void buttonZoomInSlot();
  void buttonZoomOutSlot();
  void buttonZoomFitSlot();
  void buttonChangeViewSlot();

signals:
  void openButtonClicked();
  void refreshButtonClicked();

  void zoomInClicked();
  void zoomOutClicked();
  void zoomFitClicked();

  void changeViewClicked();
};

#endif // CUSTOMTOOLBAR_H
