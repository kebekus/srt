#ifndef SRTCAMERA
#define SRTCAMERA

#warning this is a seriously flawed design
extern "C" {
#include "camera.h"
}

class srtCamera{
 public:
  srtCamera();

  struct camera camera;
};

#endif
