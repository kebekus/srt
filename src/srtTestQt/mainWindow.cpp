#include "mainWindow.h"


mainWindow::mainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  // Setup GUI
  ui.setupUi(this);
  ui.actionQuit->setShortcut(QKeySequence::Quit);
}


mainWindow::~mainWindow()
{
}
