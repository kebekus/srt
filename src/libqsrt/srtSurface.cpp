#include "srtSurface.h"

extern "C" {
#include "error.h"
#include "deriv.h"
#include "jit.h"
#include "reduce.h"
#include "value_bc.h"
}

#warning Static variables here! This is forbidden!
#warning this is not thread-safe!
static struct parser_jit *jit;
static struct parser_tree *curve_tree;
static struct parser_tree *deriv_tree[3];
static int init = 0;


srtSurface::srtSurface()
{
  _equation = QString::null;
  _errorString = QString::null;
  _errorColumn = 0;
  stripe = 0;
  curve = 0;

  if (!init) {
    jit = parser_alloc_jit((char *)value_bc, value_bc_len);
    curve_tree = parser_alloc_tree(8192);
    for (int j = 0; j < 3; j++)
      deriv_tree[j] = parser_alloc_tree(8192);
    init = 1;
  }
}

srtSurface::srtSurface(const QString &equation) 
{
  srtSurface();
  setEquation(equation);
}

void srtSurface::setEquation(const QString &equation)
{
  // Paranoia check: don't do anything if the equation did not change
  if (_equation == equation)
    return;

  _errorString = QString::null;
  _errorColumn = 0;
  _equation = equation;
#warning reset and delete stripe etc.
  
  // Paranoia check: don't do anything if the equation was empty
  if (_equation.isEmpty())
    return;

  // Now the real work starts
  if (!parser_parse(curve_tree, equation.toLatin1().constData())) {
#warning problem here. The function get_err_str() behaves horribly wrt threading
    _errorString = get_err_str();
    _errorColumn = get_err_pos();
    return;
  }
  
  if (!parser_reduce(curve_tree)) {
    _errorString = get_err_str();
    _errorColumn = get_err_pos();
    return;
  }

  for (int j = 0; j < 3; j++) {
    if (!parser_deriv(deriv_tree[j], curve_tree, token_x + j) || !parser_reduce(deriv_tree[j])) {
      _errorString = get_err_str();
      _errorColumn = get_err_pos();
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
  
  return;
}
