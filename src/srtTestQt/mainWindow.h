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

#include <QMainWindow>
#include "srtScene.h"

#include "ui_mainWindow.h"


class mainWindow : public QMainWindow
{
    Q_OBJECT

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
  mainWindow(QWidget *parent = 0);

  /**
   * Standard destructor
   *
   * The destructor hase been re-implemented to save the user settings
   */
  ~mainWindow();


 public slots:
  void equationChanged();
  void sliderMoved(int value);

  void setSampleSurface1();
  void setSampleSurface2();
  void setSampleSurface3();
  void setSampleSurface4();
  void setSampleSurface5();
  void setSampleSurface6();
  void setSampleSurface7();
  void setSampleSurface8();

 protected:
  /**
   * Re-implemented from QWidget
   */
  void closeEvent(QCloseEvent *event);


 private:
  void adjustGUItoScene();

  // GUI, as construced with the Qt designer
  Ui::mainWindow ui;

  srtScene scene;
};
