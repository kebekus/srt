#warning copyright info

#include <QCloseEvent>
#include <QSettings>
#include "mainWindow.h"


mainWindow::mainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  QSettings settings;
  
  // Setup GUI
  ui.setupUi(this);
  ui.actionQuit->setShortcut(QKeySequence::Quit);
  
  // Restore size of main window
  restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
  restoreState(settings.value("mainWindow/windowState").toByteArray());

  // Wire up GUI elements
  connect(ui.equation, SIGNAL(editingFinished()), this, SLOT(equationChanged()));
  connect(ui.aSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)) );
  sliderMoved( ui.aSlider->value() );

  // Wire up actions
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


void mainWindow::closeEvent(QCloseEvent *event)
{
  QSettings settings;
  
  // Save window geometry
  if (!isFullScreen()) {
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/windowState", saveState());
  }
  
  event->accept();
}


void mainWindow::setSampleSurface1()
{
  scene.surface.setEquation("(2*x^2 + y^2 + z^2 - 1)^3 - x^2*z^3/10 - y^2*z^3");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface2()
{
  scene.surface.setEquation("x^2 + 4 * y^2 + z^2 - 8");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface3()
{
  scene.surface.setEquation("(x^2 + y^2 + z^2 + 2)^2 - 12*(x^2 + y^2)");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface4()
{
  scene.surface.setEquation("x*y*z");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface5()
{
  scene.surface.setEquation("x^2 + y^2 + z - 1");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface6()
{
  scene.surface.setEquation("x^2 + y^2 - 1");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface7()
{
  scene.surface.setEquation("x^2 + y^2 - z^2");
  scene.surface.setA(0.0);
  adjustGUItoScene();
}


void mainWindow::setSampleSurface8()
{
  scene.surface.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  scene.surface.setA(1.0);
  adjustGUItoScene();
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


void mainWindow::adjustGUItoScene()
{
  ui.equation->setText( scene.surface.getEquation() );
  ui.equation->setCursorPosition(0);
  
  int val = qBound( ui.aSlider->minimum(), (int)floor(scene.surface.getA()*100.0+0.5), ui.aSlider->maximum() );
  ui.aSlider->setValue(val);  
}
