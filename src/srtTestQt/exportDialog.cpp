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


#include <QFileDialog>

#include "exportDialog.h"


exportDialog::exportDialog(QWidget *parent, qsrt::Scene *scene, QSize defaultSize)
  : QDialog(parent), _scene(scene)
{
  // Setup GUI
  ui.setupUi(this);
  ui.width->setValue(defaultSize.width());
  ui.height->setValue(defaultSize.height());

  connect(ui.width, SIGNAL(valueChanged(int)), this, SLOT(scaleAndCenterPreview()));
  connect(ui.height, SIGNAL(valueChanged(int)), this, SLOT(scaleAndCenterPreview()));
  if (_scene)
    ui.sceneWidget->setScene(scene);
  ui.sceneWidget->setManipulationEnabled(false);

  QTimer::singleShot(0, this, SLOT(scaleAndCenterPreview()));
}


void exportDialog::accept()
{
  QDialog::accept();

  if (_scene.isNull()) 
    return;

  QString fileName = QFileDialog::getSaveFileName(this, tr("Export Image File..."), QString::null, "Portable Network Graphic Files (*.png)");
  if (fileName.isEmpty())
    return;

// TODO: show progress dialog
  QImage img = _scene->draw(  QSize(ui.width->value(), ui.height->value()) );
  img.save(fileName);
}


void exportDialog::resizeEvent(QResizeEvent *event)
{
  scaleAndCenterPreview();
  QWidget::resizeEvent(event);
}


void exportDialog::scaleAndCenterPreview()
{
  QSize size(ui.width->value(), ui.height->value());
  QSize containerSize = ui.containerWidget->size();
  if (size.boundedTo(containerSize) != size)
    size.scale(containerSize, Qt::KeepAspectRatio);
  ui.sceneWidget->resize(size);
  ui.sceneWidget->move( (containerSize.width()-size.width())/2, (containerSize.height()-size.height())/2 );
}
