
#include <QMainWindow>

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

 private:
  // GUI, as construced with the Qt designer
  Ui::mainWindow ui;
};
