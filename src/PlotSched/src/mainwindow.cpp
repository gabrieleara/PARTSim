#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customtoolbar.h"
#include "eventview.h"

#include <QToolBar>
#include <QToolButton>
#include <QIcon>
#include <QFileDialog>

#include <QDebug>

MainWindow::MainWindow(QString folder, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  plot = new Plot(this);
  this->setCentralWidget(plot);

  populate_toolbar();
  populate_dock();

  if (folder != 0) {
    filename = folder;
    updateTitle();

    emit newFolderChosen(filename);
  }

  ep = new EventsParser;
  //connect(ep, SIGNAL(eventGenerated(QGraphicsItem*)), plot, SLOT(addNewItem(QGraphicsItem*)));
  connect(ep, SIGNAL(eventGenerated(Event)), &em, SLOT(newEventArrived(Event)));
  connect(ep, SIGNAL(fileParsed()), this, SLOT(updatePlot()));
  connect(plot, SIGNAL(zoomChanged(qreal,qreal,qreal)), this, SLOT(zoomChanged(qreal,qreal,qreal)));
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  plot = new Plot(this);
  this->setCentralWidget(plot);

  populate_toolbar();
  populate_dock();

  ep = new EventsParser;
  //connect(ep, SIGNAL(eventGenerated(QGraphicsItem*)), plot, SLOT(addNewItem(QGraphicsItem*)));
  connect(ep, SIGNAL(eventGenerated(Event)), &em, SLOT(newEventArrived(Event)));
  connect(ep, SIGNAL(fileParsed()), this, SLOT(updatePlot()));
  connect(plot, SIGNAL(zoomChanged(qreal,qreal,qreal)), this, SLOT(zoomChanged(qreal,qreal,qreal)));
}

void MainWindow::zoomChanged(qreal start, qreal end, qreal windowWidth)
{
  qreal center = em.magnify(start, end, windowWidth);
  updatePlot(center);
}

void MainWindow::populate_dock()
{
  tfl = new TraceFileLister(this);
  this->addDockWidget(Qt::LeftDockWidgetArea, tfl, Qt::Vertical);

  connect(this, SIGNAL(newFolderChosen(QString)), tfl, SLOT(update(QString)));
  connect(tfl, SIGNAL(traceChosen(QString)), this, SLOT(newTraceChosen(QString)));
}

void MainWindow::populate_toolbar()
{
  CustomToolBar * ct = new CustomToolBar(this);

  this->addToolBar(ct);

  connect(ct, SIGNAL(openButtonClicked()), this, SLOT(on_actionOpen_Folder_triggered()));
  connect(ct, SIGNAL(refreshButtonClicked()), this, SLOT(on_actionRefresh_Folder_triggered()));
  connect(ct, SIGNAL(zoomInClicked()), this, SLOT(on_actionZoomInTriggered()));
  connect(ct, SIGNAL(zoomOutClicked()), this, SLOT(on_actionZoomOutTriggered()));
  connect(ct, SIGNAL(zoomFitClicked()), this, SLOT(on_actionZoomFitTriggered()));
}


void MainWindow::on_actionZoomInTriggered()
{
  /*
  qreal center = em.magnify(0, 100, 50);
  updatePlot(center);
  */
}

void MainWindow::on_actionZoomOutTriggered()
{
  /*
  qreal center = em.magnify(0, 100, 200);
  updatePlot(center);
  */
}

void MainWindow::on_actionZoomFitTriggered()
{
  /*
  qreal center = em.magnify(0, 100, 200);
  updatePlot(center);
  */
}

MainWindow::~MainWindow()
{
  delete ep;
  delete ui;
}


void MainWindow::updateTitle()
{
  QString t = "PlotSched";
  if (filename.length() > 0) {
    t.append(" - ");
    t.append(filename);
  }
  this->setWindowTitle(t);
}


void MainWindow::on_actionOpen_triggered()
{
  QString tmpfilename = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "./",
        "Plot Sched Trace (*.pst)"
        );

  filename = tmpfilename;
  updateTitle();
}


void MainWindow::on_actionQuit_triggered()
{
  close();
}


void MainWindow::on_actionOpen_Folder_triggered()
{
  QString tmpfilename = QFileDialog::getExistingDirectory(
        this,
        tr("Open Directory"),
        "./",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

  filename = tmpfilename;
  updateTitle();

  emit newFolderChosen(filename);
}


void MainWindow::on_actionRefresh_Folder_triggered()
{}


void MainWindow::newTraceChosen(QString path)
{
  //qDebug() << "Chosen new trace : " << path;

  QFileInfo f(path);
  if (f.isFile()) {
    em.clear();
    ep->parseFile(path);
  }
}


void MainWindow::on_actionTraces_Files_triggered()
{
  tfl->setVisible(!tfl->isVisible());
}


void MainWindow::updatePlot(qreal center)
{
  plot->clear();
  unsigned long row = 0;

  PlotFrame * pf = new PlotFrame;

  QMap <QString, QList<Event>> * m = em.getCallers();
  for (QList<Event> l : *m) {
    //qDebug() << "Trovato caller";

    pf->addCaller(l.first().getCaller());

    for (Event e : l) {
      //qDebug() << " - Trovato evento";
      e.setRow(row);
      EventView * ev = new EventView(e);
      plot->addNewItem(ev);
    }
    ++row;
  }

  qreal rightmost = plot->updateSceneView(center);

  pf->setWidth(rightmost);
  plot->addNewItem(pf);

  plot->updateSceneView(center);

  qDebug() << "MainWindow::updatePlot()";
}
