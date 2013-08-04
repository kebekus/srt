#ifndef SRTSCENE
#define SRTSCENE

#include <QImage>

#include "srtCamera.h"
#include "srtSurface.h"



class srtScene{
 public:
  /**
   * Default constructor
   */
  srtScene();

#warning destructor missing

  QImage draw(QSize size);
  float a;
  int use_aabb;
  srtCamera camera;
  srtSurface surface;
};

#endif
