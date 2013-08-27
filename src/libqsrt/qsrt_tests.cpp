#include <QCoreApplication>
#include <QTest>

#include "srt_test.h"
#include "srtSurface_test.h"


int main(int argc, char *argv[]){
  QCoreApplication app(argc, argv);

  srt_test test1;
  QTest::qExec(&test1, argc, argv);

  Surface_test test2;
  QTest::qExec(&test2, argc, argv);
}
 
