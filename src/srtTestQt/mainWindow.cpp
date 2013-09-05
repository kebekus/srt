/***************************************************************************
 *   Copyright (C) 2013 Stefan Kebekus                                     *
 *   stefan.kebekus@math.uni-freiburg.de                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <math.h>
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
  ui.sceneWidget->addAction(ui.actionReset_View);

  // Restore size of main window
  restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
  restoreState(settings.value("mainWindow/windowState").toByteArray());

  // Wire up GUI elements
  connect(ui.equation, SIGNAL(editingFinished()), this, SLOT(equationTextFieldChanged()));
  connect(ui.aSlider, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)) );
  sliderMoved( ui.aSlider->value() );

  // Wire up actions
  connect( ui.actionReset_View, SIGNAL(triggered(bool)), &(scene.camera), SLOT(reset()) );
  connect( ui.actionSurface_1, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface1()) );
  connect( ui.actionSurface_2, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface2()) );
  connect( ui.actionSurface_3, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface3()) );
  connect( ui.actionSurface_4, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface4()) );
  connect( ui.actionSurface_5, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface5()) );
  connect( ui.actionSurface_6, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface6()) );
  connect( ui.actionSurface_7, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface7()) );
  connect( ui.actionSurface_8, SIGNAL(triggered(bool)), this, SLOT(setSampleSurface8()) );

  // Wire up scene, so GUI will reflect changes there
  connect(&(scene.surface), SIGNAL(aChanged()), this, SLOT(aChanged()));
  connect(&(scene.surface), SIGNAL(equationChanged()), this, SLOT(equationChanged()));
  ui.sceneWidget->setScene(&scene);

  // Initialize scene
  scene.surface.load(settings.value("mainWindow/surface"));
  scene.camera.load(settings.value("mainWindow/camera"));
  if (scene.surface.isEmpty() || scene.surface.hasError())
    setSampleSurface8();
  else {
    aChanged();
    equationChanged();
  }
}


void mainWindow::closeEvent(QCloseEvent *event)
{
  QSettings settings;
  
  // Save window geometry
  if (!isFullScreen()) {
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/windowState", saveState());
    settings.setValue("mainWindow/surface", scene.surface );
    settings.setValue("mainWindow/camera", scene.camera );
  }
  
  event->accept();
}


void mainWindow::setSampleSurface1()
{
  scene.surface.setEquation("(2*x^2 + y^2 + z^2 - 1)^3 - x^2*z^3/10 - y^2*z^3");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface2()
{
  scene.surface.setEquation("x^2 + 4 * y^2 + z^2 - 8");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface3()
{
  scene.surface.setEquation("(x^2 + y^2 + z^2 + 2)^2 - 12*(x^2 + y^2)");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface4()
{
  scene.surface.setEquation("x*y*z");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface5()
{
  scene.surface.setEquation("x^2 + y^2 + z - 1");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface6()
{
  scene.surface.setEquation("x^2 + y^2 - 1");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface7()
{
  scene.surface.setEquation("x^2 + y^2 - z^2");
  scene.surface.setA(0.0);
}


void mainWindow::setSampleSurface8()
{
  scene.surface.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
  scene.surface.setA(1.0);
}


void mainWindow::equationTextFieldChanged()
{
  scene.surface.setEquation( ui.equation->text() );
}


void mainWindow::sliderMoved(int val)
{
  qreal a = val/100.0;
  scene.surface.setA(a);
}


void mainWindow::aChanged()
{
  // Adjust the slider and label to reflect the current value of 'a'
  int val = qBound( ui.aSlider->minimum(), (int)floor(scene.surface.a()*100.0+0.5), ui.aSlider->maximum() );
  ui.aSlider->setValue(val);  
  ui.aLabel->setText( QString("a = %1").arg( scene.surface.a() ));
}


void mainWindow::equationChanged()
{
  // Set surface equation in text field
  ui.equation->setText( scene.surface.equation() );

  // React to errors
  if (scene.surface.hasError()) {
    ui.equation->setCursorPosition(scene.surface.errorIndex());
    statusBar()->showMessage(scene.surface.errorString(), 5000);
  } else
    statusBar()->clearMessage();

  // Enable slider and label only if 'a' is used in the equation
  bool aExists = scene.surface.equation().contains('a');
  ui.aSlider->setEnabled(aExists);
  ui.aLabel->setEnabled(aExists);
}
