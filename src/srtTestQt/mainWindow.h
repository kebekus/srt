
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

 private:
  // GUI, as construced with the Qt designer
  Ui::mainWindow ui;

  srtScene scene;
};
