#include "mainWindow.h"


mainWindow::mainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  // Setup GUI
  ui.setupUi(this);
  ui.actionQuit->setShortcut(QKeySequence::Quit);

  connect(ui.equation, SIGNAL(editingFinished()), this, SLOT(equationChanged()));

  connect(ui.aSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)) );
  sliderMoved( ui.aSlider->value() );

  connect( ui.actionSurface_1, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface1()) );
  connect( ui.actionSurface_2, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface2()) );
  connect( ui.actionSurface_3, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface3()) );
  connect( ui.actionSurface_4, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface4()) );
  connect( ui.actionSurface_5, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface5()) );
  connect( ui.actionSurface_6, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface6()) );
  connect( ui.actionSurface_7, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface7()) );
  connect( ui.actionSurface_8, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface8()) );


  scene.surface.setEquation(ui.equation->text());
  ui.sceneWidget->setScene(&scene);

  setSampleSurface8();
}


mainWindow::~mainWindow()
{
}


void mainWindow::setSampleSurface1()
{
  ui.equation->setText("(2*x^2 + y^2 + z^2 - 1)^3 - x^2*z^3/10 - y^2*z^3");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface2()
{
  ui.equation->setText("x^2 + 4 * y^2 + z^2 - 8");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface3()
{
  ui.equation->setText("(x^2 + y^2 + z^2 + 2)^2 - 12*(x^2 + y^2)");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface4()
{
  ui.equation->setText("x*y*z");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface5()
{
  ui.equation->setText("x^2 + y^2 + z - 1");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface6()
{
  ui.equation->setText("x^2 + y^2 - 1");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface7()
{
  ui.equation->setText("x^2 + y^2 - z^2");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::setSampleSurface8()
{
  ui.equation->setText("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  ui.equation->setCursorPosition(0);
  equationChanged();
}


void mainWindow::equationChanged()
{
  scene.surface.setEquation(ui.equation->text());

  if (scene.surface.hasError()) {
    statusBar()->showMessage(scene.surface.errorString(), 2000);
    ui.equation->setCursorPosition(scene.surface.errorIndex());
  }
}


void mainWindow::sliderMoved(int val)
{
  qreal a = val/100.0;
  ui.aLabel->setText( QString("a = %1").arg(a));
  scene.surface.setA(a);
}
