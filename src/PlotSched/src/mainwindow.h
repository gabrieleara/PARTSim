#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tracefilelister.h"
#include "eventsparser.h"
#include "plot.h"
#include "eventsmanager.h"
#include "plotframe.h"

namespace Ui {
  class MainWindow;
}

enum VIEWS { CORES, TASKS };

class MainWindow : public QMainWindow
{
  Q_OBJECT

  Ui::MainWindow *ui;

  EventsManager em;
  QString filename;
  TraceFileLister * tfl;
  EventsParser * ep;
  Plot * plot;

  enum VIEWS _currentView;

  void updateTitle();
  void populate_toolbar();
  void populate_dock();

  void loadSettings();
  void setupShortcut();
public:
  MainWindow(QString folder, QWidget *parent = 0);
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

public slots:
  void newTraceChosen(QString);
  void updatePlot(qreal center = 0);
  void zoomChanged(qreal, qreal, qreal);

private slots:
  void on_actionQuit_triggered();
  void on_actionOpen_triggered();
  void on_actionOpen_Folder_triggered();
  void on_actionRefresh_Folder_triggered();

  void on_actionZoomInTriggered();
  void on_actionZoomOutTriggered();
  void on_actionZoomFitTriggered();

  void on_actionTraces_Files_triggered();

  void on_actionViewChangedTriggered();

signals:
  void newFolderChosen(QString);
};

#endif // MAINWINDOW_H
