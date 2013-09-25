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

#include <QtPlugin>
#include <srtQt/SceneWidget.h>

#include "SceneWidgetPlugin.h"


SceneWidgetPlugin::SceneWidgetPlugin(QObject *parent)
    : QObject(parent)
{
}


void SceneWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
}


bool SceneWidgetPlugin::isInitialized() const
{
  return true;
}


QWidget *SceneWidgetPlugin::createWidget(QWidget *parent)
{
  srtQt::SceneWidget *wdgPtr = new srtQt::SceneWidget(parent);
  srtQt::Scene *sc = new srtQt::Scene(wdgPtr);
  sc->surface.setEquation("81*(x^3+y^3+z^3) - 189*(x^2*y+x^2*z+y^2*x+y^2*z+z^2*x+z^2*y) + 54*(x*y*z) + 126*(x*y+x*z+y*z) - 9*(x^2+y^2+z^2) - 9*(x+y+z) + 1");
  wdgPtr->setScene(sc);
  return wdgPtr;
}


QString SceneWidgetPlugin::name() const
{
  return "srtQt::SceneWidget";
}


QString SceneWidgetPlugin::group() const
{
  return "Display Widgets [srt]";
}


QIcon SceneWidgetPlugin::icon() const
{
  return QIcon(":/logo.png");
}


QString SceneWidgetPlugin::toolTip() const
{
  return "srtSceneWidget - shows an image of an algebraic surface in three-dimensional space";
}


QString SceneWidgetPlugin::whatsThis() const
{
  return "This srtSceneWidget displays a photo-realistic image of an algebraic surface in "
    "three-dimensional space. Please refer to the reference documentation of srtQt for "
    "more information.";
}


bool SceneWidgetPlugin::isContainer() const
{
  return false;
}


QString SceneWidgetPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"srtQt::SceneWidget\" name=\"sceneWidget\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>100</width>\n"
           "    <height>100</height>\n"
           "   </rect>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}


QString SceneWidgetPlugin::includeFile() const
{
  return "srtSceneWidget.h";
}


Q_EXPORT_PLUGIN2(scenewidgetplugin, SceneWidgetPlugin)
