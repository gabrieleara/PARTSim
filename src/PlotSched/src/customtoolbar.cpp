#include "customtoolbar.h"

#include <QToolButton>

CustomToolBar::CustomToolBar(QWidget * parent) :
  QToolBar(parent)
{
  QToolButton * buttonOpen = new QToolButton(this);
  buttonOpen->setIcon(QIcon::fromTheme("document-open"));
  this->addWidget(buttonOpen);

  connect(buttonOpen, SIGNAL(clicked()), this, SLOT(buttonOpenSlot()));


  QToolButton * buttonRefresh = new QToolButton(this);
  buttonRefresh->setIcon(QIcon::fromTheme("view-refresh"));
  this->addWidget(buttonRefresh);

  connect(buttonRefresh, SIGNAL(clicked()), this, SLOT(buttonRefreshSlot()));


  this->addSeparator();


  QToolButton * buttonZoomIn = new QToolButton(this);
  buttonZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
  this->addWidget(buttonZoomIn);

  connect(buttonZoomIn, SIGNAL(clicked()), this, SLOT(buttonZoomInSlot()));


  QToolButton * buttonZoomOut = new QToolButton(this);
  buttonZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
  this->addWidget(buttonZoomOut);

  connect(buttonZoomOut, SIGNAL(clicked()), this, SLOT(buttonZoomOutSlot()));


  QToolButton * buttonZoomFit = new QToolButton(this);
  buttonZoomFit->setIcon(QIcon::fromTheme("zoom-fit-best"));
  this->addWidget(buttonZoomFit);

  connect(buttonZoomFit, SIGNAL(clicked()), this, SLOT(buttonZoomFitSlot()));

  QToolButton * buttonChangeView = new QToolButton(this);
  buttonChangeView->setIcon(QIcon::fromTheme("zoom-fit-best")); // don't know..
  buttonChangeView->setToolTip("Change scheduling view: tasks <-> cores");
  this->addWidget(buttonChangeView);
  connect(buttonChangeView, SIGNAL(clicked()), this, SLOT(buttonChangeViewSlot()));

  this->addSeparator();
}

void CustomToolBar::buttonOpenSlot()
{
  emit openButtonClicked();
}

void CustomToolBar::buttonRefreshSlot()
{
  emit refreshButtonClicked();
}

void CustomToolBar::buttonZoomInSlot()
{
  emit zoomInClicked();
}

void CustomToolBar::buttonZoomOutSlot()
{
  emit zoomOutClicked();
}

void CustomToolBar::buttonZoomFitSlot()
{
  emit zoomFitClicked();
}

void CustomToolBar::buttonChangeViewSlot()
{
  emit changeViewClicked();
}
