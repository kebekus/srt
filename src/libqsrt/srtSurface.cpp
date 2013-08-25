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
  
  if (_setA(a))
    emit changed();

  return;
}


bool srtSurface::_setA(qreal a)
{
  // Paranoia check: don't do anything if the equation did not change
  if (a == _a)
    return false;
  
  _a = a;
  return true;
}


void srtSurface::setEquation(const QString &equation)
{
  // Get write access to private members
  QWriteLocker privatMemberLocker(&privateMemberLock);

  if (_setEquation(equation))
    emit changed();

  return;
}


bool srtSurface::_setEquation(const QString &equation)
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

  parser_reset_jit(jit);
  parser_jit_build(jit, curve_tree, "curve_xyza");
  parser_jit_build(jit, deriv_tree[0], "deriv_x");
  parser_jit_build(jit, deriv_tree[1], "deriv_y");
  parser_jit_build(jit, deriv_tree[2], "deriv_z");
  parser_jit_link(jit);

  stripe = (int64_t (*)(struct stripe_data *, int))parser_jit_func(jit, "stripe");
  curve = (v4sf (*)(m34sf, float))parser_jit_func(jit, "curve");
  
  return true;
}


qreal srtSurface::a()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  return _a;
}


QString srtSurface::equation()
{
  // Get read access to private members
  QReadLocker privatMemberLocker(&privateMemberLock);

  if (_equation.isEmpty())
    return QString::null;

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


srtSurface::operator QByteArray()
{
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QIODevice::WriteOnly);

  QDataStream out(&buffer);
  out << *this;

  return byteArray;
}


void srtSurface::load(QByteArray ar)
{
  QBuffer buffer(&ar);
  buffer.open(QIODevice::ReadOnly);

  QDataStream in(&buffer);
  in >> *this;
}


void srtSurface::construct()
{
  _clear();

  // Initialize JIT parser interna. 
#warning I do not properly understand what that is. How do we ever free this data?
  jit           = parser_alloc_jit((char *)value_bc, value_bc_len);
  curve_tree    = parser_alloc_tree(8192);
  deriv_tree[0] = parser_alloc_tree(8192);
  deriv_tree[1] = parser_alloc_tree(8192);
  deriv_tree[2] = parser_alloc_tree(8192);
}


void srtSurface::_clear()
{
  _equation    = QString::null;
  _errorString = QString::null;
  _errorIndex  = 0;
  _a           = 0;
  stripe       = 0;
  curve        = 0;
}


QDataStream & operator<< (QDataStream& out, srtSurface& surface)
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


QDataStream & operator>> (QDataStream& in, srtSurface& surface)
{
  // Get write access to private members, then write them to the stream
  QWriteLocker privatMemberLocker(&surface.privateMemberLock);

  bool changed = false;

  // Read and check the header
  quint16 magic;
  in >> magic;
  if (magic != 1341) {
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
    surface._clear();
    surface._errorString = "I/O error";
    surface._errorIndex  = -1;
    surface.touch();
    return in;
  }

  // Write the data to the surface
  changed |= surface._setEquation(n_equation);
  if (surface._errorString.isEmpty()) {
    changed |= (surface._errorString != n_errorString);
    changed |= (surface._errorIndex != n_errorIndex);

    surface._errorString = n_errorString;
    surface._errorIndex  = n_errorIndex;
  }
  changed |= surface._setA(n_a);

  if (changed)
    surface.touch();
  
  in.setVersion(oldVersion);
  return in;
}


bool operator== (srtSurface& s1, srtSurface& s2)
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


bool operator!= (srtSurface& s1, srtSurface& s2)
{
  return !(s1 == s2);
}
