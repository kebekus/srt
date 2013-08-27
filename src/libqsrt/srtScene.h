#ifndef SRTSCENE
#define SRTSCENE

#include <QImage>
#include <QObject>

#include "srtCamera.h"
#include "srtSurface.h"



class srtScene : public QObject
{
  Q_OBJECT


 public:
  /**
   * Default constructor
   */
  srtScene(QObject *parent=0);

#warning destructor missing

 
 signals:
  void changed();

 public:

  QImage draw(QSize size);
  int use_aabb;
  srtCamera camera;
  qsrt::Surface surface;
};

#endif
