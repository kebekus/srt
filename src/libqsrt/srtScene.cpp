#include <QtCore>
#include <SDL.h>

#include "srtScene.h"

extern "C" {
#include "aabb.h"
#include "camera.h"
#include "sphere.h"
#include "stripe_data.h"
}


srtScene::srtScene() 
{
  a = 1.0;
  use_aabb = 0;
}

class task {
public:
  struct stripe_data *sd;
  srtSurface *surface;
  int line;
};

void runTask(task &tsk)
{
  tsk.surface->stripe(tsk.sd, tsk.line);
}

QImage srtScene::draw(QSize size)
{
  // Paranoia check: if size is empty or invalid, return an empty image.
  if (size.isEmpty())
    return QImage();

  QImage img(size, QImage::Format_ARGB32);

  
  struct sphere sphere = { v4sf_set3(0, 0, 0), 3 };
  struct aabb aabb = { v4sf_set3(-3, -3, -3), v4sf_set3(3, 3, 3) };

#warning This is WRONG.
  uint32_t *fb = (uint32_t *)img.scanLine(0);

  int w = img.width();
  int h = img.height();
  float dw = 2.0f / (float)w;
  float dh = -2.0f / (float)h;
  v4sf dU = v4sf_set1(dw) * camera.camera.right;
  v4sf dV = v4sf_set1(dh) * camera.camera.up;
  m34sf zU = m34sf_set(v4sf_set1(0), dU, v4sf_set1(0), dU);
  m34sf zV = m34sf_set(v4sf_set1(0), v4sf_set1(0), dV, dV);
  m34sf U = m34sf_subv(zU, camera.camera.right);
  m34sf V = m34sf_addv(zV, camera.camera.up);
  m34sf UV = m34sf_add(U, V);
  struct stripe_data sdata = { fb, w, h, dU, dV, UV, sphere, aabb, camera.camera, a, use_aabb };

  int numTasks = qMax(1 , QThread::idealThreadCount());
  
  QVector<task> tskList(h/2);
  for (int i = 0; i < h/2; i++) {
    tskList[i].surface = &surface;
    tskList[i].sd = &sdata;
    tskList[i].line = 2 * i;
  }
  
  // Run tasks concurrently
  QtConcurrent::blockingMap(tskList, runTask);
  return img;
}
