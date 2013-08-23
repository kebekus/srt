#include <QtCore>
#include <SDL.h>

#include "srtScene.h"

extern "C" {
#include "aabb.h"
#include "camera.h"
#include "sphere.h"
#include "stripe_data.h"
}


srtScene::srtScene(QObject *parent) 
  : QObject(parent)
{
  use_aabb = 0;

  connect(&surface, SIGNAL(changed()), this, SIGNAL(changed()));
}


class task {
public:
  int64_t (*stripe)(struct stripe_data *sd, int j);
  struct stripe_data *sd;
  srtSurface *surface;
  int line;
};


void runTask(task &tsk)
{
  tsk.stripe(tsk.sd, tsk.line);
}


QImage srtScene::draw(QSize size)
{
  // Paranoia check: if size is empty or invalid, return a null image.
  if (size.isEmpty())
    return QImage();

  // Paranoia check: if surface has an error, return a null image.
  if (surface.hasError()) 
    return QImage();
    
  QImage img(size, QImage::Format_ARGB32);

  // Paranoia check: if surface is empty, return an empty image.
  if (surface.isEmpty()) {
    img.fill(Qt::black);
#warning clear image
    return img;
  }

  struct sphere sphere = { v4sf_set3(0, 0, 0), 3 };
  struct aabb aabb = { v4sf_set3(-3, -3, -3), v4sf_set3(3, 3, 3) };
#warning This is WRONG.
  uint32_t *fb = (uint32_t *)img.scanLine(0);

  struct camera _camera = init_camera();

  int w = img.width();
  int h = img.height();

  // Copied from Inan's srt.cpp. No idea what's going on here.
  float dw = 2.0f / (float)h;
  float dh = -2.0f / (float)h;
  float ow = (float)w / (float)h;
  float oh = 1;
  if (w < h) {
    dw = 2.0f / (float)w;
    dh = -2.0f / (float)w;
    ow = 1;
    oh = (float)h / (float)w;
  }
  v4sf dU = v4sf_set1(dw) * _camera.right;
  v4sf dV = v4sf_set1(dh) * _camera.up;
  v4sf oU = v4sf_set1(ow) * _camera.right;
  v4sf oV = v4sf_set1(oh) * _camera.up;
  m34sf zU = m34sf_set(v4sf_set1(0), dU, v4sf_set1(0), dU);
  m34sf zV = m34sf_set(v4sf_set1(0), v4sf_set1(0), dV, dV);
  m34sf U = m34sf_subv(zU, oU);
  m34sf V = m34sf_addv(zV, oV);
  m34sf UV = m34sf_add(U, V);

  // Get read access to private members of the surface
  QReadLocker privatMemberLocker(&surface.privateMemberLock);

  struct stripe_data sdata = { fb, w, h, dU, dV, UV, sphere, aabb, _camera, (float) surface._a, use_aabb };

  int numTasks = qMax(1 , QThread::idealThreadCount());


  QVector<task> tskList(h/2);
  for (int i = 0; i < h/2; i++) {
    tskList[i].stripe  = surface.stripe;
    tskList[i].surface = &surface;
    tskList[i].sd      = &sdata;
    tskList[i].line    = 2 * i;
  }
  
  // Run tasks concurrently
  QtConcurrent::blockingMap(tskList, runTask);
  return img;
}
