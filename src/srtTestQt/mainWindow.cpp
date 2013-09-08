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
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "exportDialog.h"
#include "mainWindow.h"


mainWindow::mainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  QSettings settings;
  
  // Setup GUI
  ui.setupUi(this);
  ui.actionOpen->setShortcut(QKeySequence::Open);
  ui.actionSave->setShortcut(QKeySequence::Save);
  ui.actionSaveAs->setShortcut(QKeySequence::SaveAs);
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
  connect( ui.actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()) );
  connect( ui.actionSave, SIGNAL(triggered(bool)), this, SLOT(save()) );
  connect( ui.actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs()) );
  connect( ui.actionExport, SIGNAL(triggered(bool)), this, SLOT(exportImage()) );
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
  ui.sceneWidget->load(settings.value("mainWindow/sceneWidget"));
  scene.load(settings.value("mainWindow/scene"));
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
  }
  
  settings.setValue("mainWindow/scene", scene );
  settings.setValue("mainWindow/sceneWidget", ui.sceneWidget->settings() );

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


void mainWindow::exportImage()
{
  // switch off rotation, but remember rotation status
  bool rot = ui.sceneWidget->rotation();
  ui.sceneWidget->setRotation(false);

  exportDialog dialog(this, &scene, ui.sceneWidget->size());
  dialog.exec();

  // Restore rotation
  ui.sceneWidget->setRotation(rot);
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


void mainWindow::open()
{
  QString nfileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString::null, tr("srt Scene Files (*.ssc)"));
  if (nfileName.isEmpty())
    return;

  QString magicID;
  QVariantMap map;

  QFile file(nfileName);
  file.open(QIODevice::ReadOnly);
  QDataStream in(&file);
  in.setVersion(QDataStream::Qt_4_8);
  in >> magicID;
  in >> map;
  file.close();

  QString titleLine("Error Opening File");
  if (in.status() != QDataStream::Ok) {
    QMessageBox::warning(this, titleLine, tr("There was an error reading from the file <b>%1</b>.").arg(nfileName));
    return;
  }
  if ((magicID != "srtScene") || !map.contains("scene")) {
    QMessageBox::warning(this, titleLine, tr("There was an error reading from the file <b>%1</b>. The file does not seem to be of the right format.").arg(nfileName));
    return;
  }

  fileName = nfileName;
  ui.actionSave->setEnabled(true);

  scene.load(map["scene"]);
  if (map.contains("srtTestQt/sceneWidget"))
    ui.sceneWidget->load(map["srtTestQt/sceneWidget"]);
}


void mainWindow::save()
{
  if (fileName.isEmpty())
    return;

  QVariantMap map;
  map["scene"]  = scene;
  map["srtTestQt/sceneWidget"] = ui.sceneWidget->settings();
  
  QFile file(fileName);
  file.open(QIODevice::WriteOnly);
  QDataStream out(&file);
  out.setVersion(QDataStream::Qt_4_8);
  out << QString("srtScene");
  out << map;
  file.close();

  if (out.status() != QDataStream::Ok)
    QMessageBox::warning(this, tr("Error Saving File"), tr("There was an error writing to the file <b>%1</b>. No useful data has been written.").arg(fileName));
}


void mainWindow::saveAs()
{
  bool rot = ui.sceneWidget->rotation();
  ui.sceneWidget->setRotation(false);
  QString nfileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString::null, tr("srt Scene Files (*.ssc)"));
  if (nfileName.isEmpty()) {
    ui.sceneWidget->setRotation(rot);
    return;
  }

  fileName = nfileName;
  ui.actionSave->setEnabled(true);
  save();
  ui.sceneWidget->setRotation(rot);
}
