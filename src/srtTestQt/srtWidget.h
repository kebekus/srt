#ifndef SRTWIDGET
#define SRTWIDGET

#include <QFrame>
#include <QPointer>

#include "srtScene.h"

class srtWidget : public QFrame
{
    Q_OBJECT

 public:
  srtWidget(QWidget *parent = 0);

  ~srtWidget();

  void setScene(srtScene *_scene);

  virtual void paintEvent ( QPaintEvent * event );

 private:
  QPointer<srtScene> scene;
};

#endif
