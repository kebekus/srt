#warning copyright notice missing

#ifndef SRTSURFACE
#define SRTSURFACE

#include <QString>

#warning the user of libqsrt should never include this
extern "C" {
#include "vector.h"
#include "matrix.h"
#include "parser.h"
}

#warning documentation missing

class srtSurface{
 public:
  /**
   * Default constructor
   *
   * Construct an empty surface
   */
  srtSurface();

  srtSurface(const QString &equation);

#warning destructor missing

#warning copy constructor missing

#warning implicit sharing missing

#warning I/O missing

#warning this is not thread-safe! 
  void setEquation(const QString &equation);

#warning this is not thread-safe! 
  QString getEquation() const {return _equation;}

#warning this is not thread-safe! 
  bool hasError() const {return !_errorString.isEmpty();}

#warning this is not thread-safe! 
  bool isEmpty() const {return !_equation.isEmpty();}

#warning this is not thread-safe! Far from it!
#warning this behaves terribly with respect to copying
#warning this should not be left open
#warning non-qt types used here which should be hidden
  int64_t (*stripe)(struct stripe_data *sd, int j);
#warning this is not thread-safe! Far from it!
#warning this behaves terribly with respect to copying
#warning this should not be left open
#warning non-qt types used here which should be hidden
  v4sf (*curve)(m34sf v, float a);

#warning this is not thread-safe! 
  QString errorString() const {return _errorString;}

#warning this is not thread-safe! 
  unsigned int errorColumn() const {return _errorColumn;}

 private:
#warning mutex missing
  // The equation that is currently set. If empty, then the surface is
  // empty. The member 'errorString' below can be used to find out if the
  // equation could be compiled into machine code.
  QString _equation;
  
  // If not empty, then an error occured in the method setEquation(). This
  // string describes the error in natural language
#warning This is flawed design and will lead to problems with translations. Error codes should be used or translations provided.
  QString _errorString;
  
  // If _errorString is not empty, then this member points to the index in
  // _equation where the error occurred.
  unsigned int _errorColumn;
};

#endif
