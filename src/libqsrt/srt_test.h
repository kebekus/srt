#warning copyright info 

#include <QtTest/QtTest>

 class srt_test : public QObject
 {
   Q_OBJECT
     
  private slots:
   void parser_errorReporting();
   void parser_floatParsing();
   void parser_constantA();
 };
