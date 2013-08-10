#include <QCoreApplication>
#include <QTest>

#include "srtSurface_test.h"


int main(int argc, char *argv[]){
  QCoreApplication app(argc, argv);

  srtSurface_test test1;
  QTest::qExec(&test1, argc, argv);
}
 
