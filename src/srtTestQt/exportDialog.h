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

#include <QDialog>
#include <srtQt/Scene.h>

#include "ui_exportDialog.h"


class exportDialog : public QDialog
{
  Q_OBJECT;

 public:
  /**
   * Standard constructor
   *
   * Constructs the GUI and all necessary objects, and reads in user settings
   * from the last save operation.
   *
   * @param parent This parameter is passed on to the constructor of the
   * QMainWindow
   */
  exportDialog(QWidget *parent, srtQt::Scene *scene, QSize defaultSize);

 protected:
  void accept();

 private slots:
  void resizeEvent(QResizeEvent *event);
  void scaleAndCenterPreview();

 private:
  // GUI, as construced with the Qt designer
  Ui::exportDialog ui;
  QPointer<srtQt::Scene> _scene;
};
