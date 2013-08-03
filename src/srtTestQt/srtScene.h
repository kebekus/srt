#ifndef SRTSURFACE
#define SRTSURFACE

#include <QString>

extern "C" {
#include "vector.h"
#include "matrix.h"
#include "parser.h"
}


class srtSurface{
 public:
  /**
   * Default constructor
   *
   * Construct an empty surface
   */
  srtSurface();

#warning destructor missing

#warning this is not thread-safe! 
  bool setEquation(const QString &equation);

#warning this is not thread-safe! 
  QString getEquation() const {return _equation;}

#warning this is not thread-safe! 
  bool hasError() const {return !_errorString.isEmpty();}

#warning this is not thread-safe! 
  bool isEmpty() const {return !_equation.isEmpty();}

#warning this is not thread-safe! Far from it
  int64_t (*stripe)(struct stripe_data *sd, int j);
  v4sf (*curve)(m34sf v, float a);

#warning this is not thread-safe! 
  QString errorString() const {return _errorString;}

#warning this is not thread-safe! 
  unsigned int errorColumn() const {return _errorColumn;}
 private:  
  QString _equation;

  // If empty, then the surface could be constructed properly. If not, then this
  // string contains a message why the srtSurface object could not be created.
  QString _errorString;
  
  unsigned int _errorColumn;
};

#endif
