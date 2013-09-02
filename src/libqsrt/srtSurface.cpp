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
#include "srtSurface.h"

extern "C" {
#include "error.h"
#include "deriv.h"
#include "reduce.h"
#include "value_bc.h"
}
#include "jit.h"

namespace qsrt {

// Static Variables
QMutex      Surface::parserSerialization;


Surface::Surface(QObject *parent)
  : QObject(parent), privateMemberLock(QReadWriteLock::Recursive)
{
  construct();
  connect(this, SIGNAL(equationChanged()), this, SIGNAL(changed()));
  connect(this, SIGNAL(aChanged()), this, SIGNAL(changed()));
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
  
  parser_free_tree(curve_tree);
  parser_free_tree(deriv_tree[0]);
  parser_free_tree(deriv_tree[1]);
  parser_free_tree(deriv_tree[2]);
  delete jit;
}


void Surface::clear()
{
  // Get write access to private members
  privateMemberLock.lockForWrite();
  _clear();
  privateMemberLock.unlock();

  return;
}


bool Surface::_clear()
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
#warning reset and delete stripe etc.
 
  // Paranoia check: do not JIT compile if the new equation is empty
  if (_equation.isEmpty())
    return true;

  // Now the real work starts
#warning This is really bad. A better solution should be found.
  QMutexLocker parserLocker(&parserSerialization);
  if (!parser_parse(curve_tree, equation.toLatin1().constData())) {
    _errorString = get_err_str();
    _errorIndex  = get_err_pos();
    return true;
  }
  
  if (!parser_reduce(curve_tree)) {
    _errorString = get_err_str();
    _errorIndex  = get_err_pos();
    return true;
  }

  for (int j = 0; j < 3; j++) {
    if (!parser_deriv(deriv_tree[j], curve_tree, token_x + j) || !parser_reduce(deriv_tree[j])) {
      _errorString = get_err_str();
      _errorIndex  = get_err_pos();
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


Surface::operator QByteArray()
{
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);

  QDataStream out(&buffer);
  out << *this;

  return byteArray;
}


void Surface::load(QByteArray ar)
{
  QBuffer buffer(&ar);
  buffer.open(QIODevice::ReadOnly);

  QDataStream in(&buffer);
  in >> *this;
}


void Surface::construct()
{
  _equation    = QString::null;
  _errorString = QString::null;
  _errorIndex  = 0;
  _a           = 0;
  stripe       = 0;

  // Initialize JIT parser interna. 
  jit           = new parser::jit((char *)value_bc, value_bc_len);
  curve_tree    = parser_alloc_tree(8192);
  deriv_tree[0] = parser_alloc_tree(8192);
  deriv_tree[1] = parser_alloc_tree(8192);
  deriv_tree[2] = parser_alloc_tree(8192);
}


QDataStream & operator<< (QDataStream& out, qsrt::Surface& surface)
{
  // Write a header with a "magic number" and a version
  out << (quint16)1341;
  out << (quint16)1;
  
  // Set I/O version, remember original version number
  int oldVersion = out.version();
  out.setVersion(QDataStream::Qt_4_8);

  // Get read access to private members, then write them to the stream
  QReadLocker privatMemberLocker1(&surface.privateMemberLock);
  out << surface._equation;
  out << surface._errorString;
  out << surface._errorIndex;
  out << surface._a;

  // Restore I/O version
  out.setVersion(oldVersion);
  return out;
}


QDataStream & operator>> (QDataStream& in, qsrt::Surface& surface)
{
  // Read and check the header
  quint16 magic;
  in >> magic;
  if (magic != 1341) {
    // Get write access to private members, then write them to the stream
    QWriteLocker privatMemberLocker(&surface.privateMemberLock);

    surface._clear();
    surface._errorString = "bad file format";
    surface._errorIndex  = -1;
    surface.touch();
    return in;
  }

  // Read the version
  quint16 version;
  in >> version;
  if (version > 1) {
    // Get write access to private members, then write them to the stream
    QWriteLocker privatMemberLocker(&surface.privateMemberLock);

    surface._clear();
    surface._errorString = "file format too new";
    surface._errorIndex  = -1;
    surface.touch();
    return in;
  }
  
  int oldVersion = in.version();
  in.setVersion(QDataStream::Qt_4_8);
  
  // Read the data
  QString n_equation;
  QString n_errorString;
  int     n_errorIndex;
  qreal   n_a;

  in >> n_equation;
  in >> n_errorString;
  in >> n_errorIndex;
  in >> n_a;

  if (in.status() != QDataStream::Ok) {
    // Get write access to private members, then write them to the stream
    QWriteLocker privatMemberLocker(&surface.privateMemberLock);

    surface._clear();
    surface._errorString = "I/O error";
    surface._errorIndex  = -1;
    surface.touch();
    return in;
  }

  // Get write access to private members and apply changes
  surface.privateMemberLock.lockForWrite();
  surface._setEquation(n_equation);
  surface._errorString = n_errorString;
  surface._errorIndex  = n_errorIndex;
  surface._setA(n_a);
  surface.privateMemberLock.unlock();

  in.setVersion(oldVersion);

  return in;
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

} // namespace qsrt
