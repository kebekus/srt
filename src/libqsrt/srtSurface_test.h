#warning copyright info

 #include <QtTest/QtTest>

 class srtSurface_test : public QObject
 {
   Q_OBJECT
   
public slots:
   void increaseCounter();
  
private slots:
   void benchmark();
   void specialStates();
   void constructor();
   void equationProperty();
   void aProperty();

 private:
   int counter;
 };
