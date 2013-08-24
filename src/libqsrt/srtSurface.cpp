#warning copyright missing
#include "srtSurface.h"

extern "C" {
#include "error.h"
#include "deriv.h"
#include "jit.h"
#include "reduce.h"
#include "value_bc.h"
}

// Static Variables
QMutex      srtSurface::parserSerialization;


srtSurface::srtSurface(QObject *parent)
  : QObject(parent), privateMemberLock(QReadWriteLock::Recursive)
{
  construct();
}


srtSurface::srtSurface(const QString &equation, qreal a, QObject *parent)
  : QObject(parent), privateMemberLock(QReadWriteLock::Recursive)
{
  construct();
  setEquation(equation);
  setA(a);
}


srtSurface::~srtSurface()
{
  // Get write access to private members. Wait till the last reader has finished
  QWriteLocker locker(&privateMemberLock);
  
#warning I do not properly understand what that is. Is this correct?
  parser_free_tree(curve_tree);
  parser_free_tree(deriv_tree[0]);
  parser_free_tree(deriv_tree[1]);
  parser_free_tree(deriv_tree[2]);
  parser_free_jit(jit);
}


void srtSurface::setA(qreal a)
{
  // Get write access to private members
  QWriteLocker privatMemberLocker(&privateMemberLock);
  
  // Paranoia check: don't do anything if the equation did not change
  if (a == _a)
    return;
  
  _a = a;
  emit changed();
  return;
}


void srtSurface::setEquation(const QString &equation)
{
  // Get write access to private members
  QWriteLocker privatMemberLocker(&privateMemberLock);

  // Paranoia check: don't do anything if the equation did not change
  if (_equation == equation)
    return;

  _errorString = QString::null;
  _errorIndex  = 0;
  _equation    = equation;
#warning reset and delete stripe etc.
 
  // Paranoia check: don't do anything if the equation was empty
  if (_equation.isEmpty()) {
    emit changed();
    return;
  }

  // Now the real work starts
#warning This is really bad. A better solution should be found.
  QMutexLocker parserLocker(&parserSerialization);
  if (!parser_parse(curve_tree, equation.toLatin1().constData())) {
    _errorString = get_err_str();
    _errorIndex  = get_err_pos();
    emit changed();
    return;
  }
  
  if (!parser_reduce(curve_tree)) {
    _errorString = get_err_str();
    _errorIndex  = get_err_pos();
    emit changed();
    return;
  }

  for (int j = 0; j < 3; j++) {
    if (!parser_deriv(deriv_tree[j], curve_tree, token_x + j) || !parser_reduce(deriv_tree[j])) {
      _errorString = get_err_str();
      _errorIndex  = get_err_pos();
      emit changed();
      return;
    }
  }

  parser_reset_jit(jit);
  parser_jit_build(jit, curve_tree, "curve_xyza");
  parser_jit_build(jit, deriv_tree[0], "deriv_x");
  parser_jit_build(jit, deriv_tree[1], "deriv_y");
  parser_jit_build(jit, deriv_tree[2], "deriv_z");
  parser_jit_link(jit);

  stripe = (int64_t (*)(struct stripe_data *, int))parser_jit_func(jit, "stripe");
  curve = (v4sf (*)(m34sf, float))parser_jit_func(jit, "curve");
  
  emit changed();
  return;
}


qreal srtSurface::getA()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _a;
}


QString srtSurface::getEquation()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _equation;
}


bool srtSurface::hasError()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return !_errorString.isEmpty();
}


bool srtSurface::isEmpty()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _equation.isEmpty();
}


QString srtSurface::errorString() 
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _errorString;
}


int srtSurface::errorIndex()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  if (hasError())
    return _errorIndex;

  return 0;
}


void srtSurface::construct()
{
  _equation    = QString::null;
  _errorString = QString::null;
  _errorIndex  = 0;
  _a           = 0;
  stripe       = 0;
  curve        = 0;

  // Initialize JIT parser interna. 
#warning I do not properly understand what that is. How do we ever free this data?
  jit           = parser_alloc_jit((char *)value_bc, value_bc_len);
  curve_tree    = parser_alloc_tree(8192);
  deriv_tree[0] = parser_alloc_tree(8192);
  deriv_tree[1] = parser_alloc_tree(8192);
  deriv_tree[2] = parser_alloc_tree(8192);
}
