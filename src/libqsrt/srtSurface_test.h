 #include <QtTest/QtTest>

 class srtSurface_test : public QObject
 {
   Q_OBJECT
   
public slots:
   void increaseCounter();
  
private slots:
   void benchmark_JITinitialization();
   void benchmark_JITcompilation();

   void specialStates();
   void constructor();
   void equationProperty();

   

 private:
   int counter;
 };
