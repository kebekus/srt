#include "srtQt/Surface.h"

int main(int argc, char **argv)
{
  srtQt::Surface s1;

  for(int i=0; i<20; i++) {
    s1.setEquation("x^2+z^2-y^2-1");
    s1.setEquation("x^2+z^2-y^2-2");
  }

  return 0;
}
