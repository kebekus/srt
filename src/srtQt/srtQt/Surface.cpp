/***************************************************************************
 *   Copyright (C) 2013 Stefan Kebekus                                     *
 *   stefan.kebekus@math.uni-freiburg.de                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QBuffer>
#include <QDebug>
#include "srtQt/Surface.h"

#include "error.h"
#include "deriv.h"
#include "reduce.h"
#include "value_bc.h"
#include "jit.h"

namespace srtQt {

// Static Variables
QMutex      Surface::parserSerialization;


Surface::Surface(QObject *parent)
  : QObject(parent), privateMemberLock(QReadWriteLock::Recursive)
{
  construct();
}


Surface::Surface(const QString &equation, qreal a, QObject *parent)
  : QObject(parent), privateMemberLock(QReadWriteLock::Recursive)
{
  construct();
  setEquation(equation);
  setA(a);
}


Surface::~Surface()
{
  // Get write access to private members. Wait till the last reader has finished
  QWriteLocker locker(&privateMemberLock);
  
  srt::free_tree(curve_tree);
  srt::free_tree(deriv_tree[0]);
  srt::free_tree(deriv_tree[1]);
  srt::free_tree(deriv_tree[2]);
  delete jit;
}


void Surface::reset()
{
  // Get write access to private members
  privateMemberLock.lockForWrite();
  _reset();
  privateMemberLock.unlock();

  return;
}


bool Surface::_reset()
{
  bool changed = false;

  changed |=  (_a == 0.0);
  _a = 0.0;

  changed |=  (_equation.isEmpty());
  _equation = QString::null;

  changed |=  (_errorString.isEmpty());
  _errorString = QString::null;

  changed |=  (_errorIndex == 0);
  _errorIndex = 0;

  return changed;
}


void Surface::setA(qreal a)
{
  // Get write access to private members
  privateMemberLock.lockForWrite();
  bool change = _setA(a);
  privateMemberLock.unlock();

  if (change)
    emit aChanged();

  return;
}


bool Surface::_setA(qreal a)
{
  // Paranoia check: don't do anything if the equation did not change
  if (a == _a)
    return false;
  
  _a = a;
  return true;
}


void Surface::setEquation(const QString &equation)
{
  // Get write access to private members
  privateMemberLock.lockForWrite();
  bool change = _setEquation(equation);
  privateMemberLock.unlock();

  if (change)
    emit equationChanged();

  return;
}


bool Surface::_setEquation(const QString &equation)
{
  // Paranoia check: don't do anything if the equation did not change
  if (_equation == equation)
    return false;

  _errorString = QString::null;
  _errorIndex  = 0;
  _equation    = equation;
// TODO: reset and delete stripe etc.
 
  // Paranoia check: do not JIT compile if the new equation is empty
  if (_equation.isEmpty())
    return true;

  // Now the real work starts
// TODO: This is really bad. A better solution should be found.
  QMutexLocker parserLocker(&parserSerialization);
  if (!srt::parse(curve_tree, equation.toLatin1().constData())) {
    _errorString = srt::error::get_str();
    _errorIndex  = srt::error::get_pos();
    return true;
  }
  
  if (!srt::reduce(curve_tree)) {
    _errorString = srt::error::get_str();
    _errorIndex  = srt::error::get_pos();
    return true;
  }

  for (int j = 0; j < 3; j++) {
    if (!srt::deriv(deriv_tree[j], curve_tree, srt::token_x + j) || !srt::reduce(deriv_tree[j])) {
      _errorString = srt::error::get_str();
      _errorIndex  = srt::error::get_pos();
      return true;
    }
  }

  jit->reset();
  jit->build(curve_tree, "curve_xyza");
  jit->build(deriv_tree[0], "deriv_x");
  jit->build(deriv_tree[1], "deriv_y");
  jit->build(deriv_tree[2], "deriv_z");
  jit->link();

  stripe = (int64_t (*)(struct stripe_data *, int))jit->func("stripe");
  
  return true;
}


qreal Surface::a()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _a;
}


QString Surface::equation()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  if (_equation.isEmpty())
    return QString::null;

  return _equation;
}


bool Surface::hasError()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return !_errorString.isEmpty();
}


bool Surface::isEmpty()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _equation.isEmpty();
}


QString Surface::errorString() 
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _errorString;
}


int Surface::errorIndex()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  if (hasError())
    return _errorIndex;

  return 0;
}


Surface::operator QVariant()
{
  // Get read access to private members
  QReadLocker locker(&privateMemberLock);

  QVariantMap map;
  map["equation"] = _equation;
  map["a"]        = _a;

  return map;
}


bool Surface::load(QVariant variant)
{
  QVariantMap map = variant.value<QVariantMap>();

  // Check existence of the most important members
  if ( !map.contains("equation") || !map.contains("a") )
    return false;

  QWriteLocker locker(&privateMemberLock);
  _setEquation( map["equation"].value<QString>() );
  _setA( map["a"].value<qreal>() );
  
  return true;
}


void Surface::construct()
{
  _equation    = QString::null;
  _errorString = QString::null;
  _errorIndex  = 0;
  _a           = 0;
  stripe       = 0;

  // Initialize JIT parser interna. 
  jit           = new srt::jit((char *)value_bc, value_bc_len);
  curve_tree    = srt::alloc_tree(8192);
  deriv_tree[0] = srt::alloc_tree(8192);
  deriv_tree[1] = srt::alloc_tree(8192);
  deriv_tree[2] = srt::alloc_tree(8192);

  // Wire up signals
  connect(this, SIGNAL(equationChanged()), this, SIGNAL(changed()));
  connect(this, SIGNAL(aChanged()), this, SIGNAL(changed()));
}


bool operator== (Surface& s1, Surface& s2)
{
  // Get read access to private members
  QReadLocker privatMemberLocker1(&s1.privateMemberLock);
  QReadLocker privatMemberLocker2(&s2.privateMemberLock);
  
  // Empty state
  if (s1._equation.isEmpty() != s2._equation.isEmpty())
    return false;

  // Error state
  if (s1._errorString.isEmpty() != s2._errorString.isEmpty())
    return false;
  if (!s1._errorString.isEmpty()) {
    if (s1._errorString != s2._errorString)
      return false;
    if (s1._errorIndex != s2._errorIndex)
      return false;
  }

  // Equation property
  if ( !(s1._equation.isEmpty() && s2._equation.isEmpty()) )
    if (s1._equation != s2._equation)
      return false;

  // A property
  if (s1._a != s2._a)
    return false;

  return true;
}


bool operator!= (Surface& s1, Surface& s2)
{
  return !(s1 == s2);
}

} // namespace srtQt
