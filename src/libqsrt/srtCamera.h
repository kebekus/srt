#warning copyright notice missing

#ifndef SRTCAMERA
#define SRTCAMERA

#warning This is a seriously flawed design. The user of libqsrt should never include this.
extern "C" {
#include "camera.h"
}

class srtCamera{
 public:
  srtCamera();

#warning serialisation and i/o missing
  struct camera _camera;
};

#endif
