#ifndef SRTWIDGET
#define SRTWIDGET

#include <QFrame>

#include "srtScene.h"

class srtWidget : public QFrame
{
    Q_OBJECT

 public:
  srtWidget(QWidget *parent = 0);

  ~srtWidget();

  virtual void paintEvent ( QPaintEvent * event );

 private:
  srtScene scene;
};

#endif
