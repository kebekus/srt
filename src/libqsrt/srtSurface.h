#warning copyright notice missing

#ifndef SRTSURFACE
#define SRTSURFACE

#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QString>

#warning the user of libqsrt should never include this
extern "C" {
#include "vector.h"
#include "matrix.h"
#include "parser.h"
}
#include "jit.h"

class srtScene;



/**
 * \brief Algebraic surface in three-dimensional Euclidean space.
 *
 * This class represents an algebraic surface in the three-dimensional Euclidean
 * space. Surfaces are given implicitly, as the zero-set of a polynomial in the
 * variables x, y and z. The method setEquation() is used to set the
 * equation. Once set, highly optimized machine code is created, which is used
 * to evaluate the equation and its derivaties.
 *
 * Surfaces may have special states.
 *
 * - Surfaces are empty if no equation has been set, or if an empty equation has
 *   been set. Empty surfaces can be rendered, but nothing will show. Use the
 *   method isEmpty() to check if your surface is empty.
 *
 * - If setEquation() has been called with a string that does not contain a
 *   well-formed polynomial, then an error condition will be set. Surfaces with
 *   error conditions cannot be rendered. Use the method hasError() to check if
 *   an error condition is set, and use the methods errorString() and
 *   errorIndex() to obtain a description of the error.
 *
 * All methods of the class are reentrant and thread-safe.
 *
 * @author Stefan Kebekus 
 */

class srtSurface : public QObject
{
  Q_OBJECT;

 /**
  * \brief Polynomial that defines the surface implicitly
  *
  * This string defines a polynomial in variables x, y and z, and an additional
  * constant a, for example instance "x^2+y^2-z^2-a". Use the methods
  * getEquation() and setEquation() to access the property. The signal changed()
  * is emitted whenever this property changes.
  */
  Q_PROPERTY(QString equation READ getEquation WRITE setEquation NOTIFY changed);

 /**
  * \brief Constant used in the polynomial equation
  *
  * This float defines a constant which is used when the polynomial is
  * evaluates. While setting a new equation string is slow because of the JIT
  * compiler involved, changing the constant a is extremely fast. Use the
  * methods getA() and setA() to access the property. The signal changed() is
  * emitted whenever this property changes.
  */
  Q_PROPERTY(qreal a READ getA WRITE setA NOTIFY changed);

 public:
  /**
   * \brief Default constructor
   *
   * @param parent Standard argument for the QObject constructor
   *
   * Construct an empty surface. With the constant a set to 0.0. Because this
   * constructor also initializes the JIT compiler, the constructor is not very
   * fast. On an Intel(R) Core(TM) i7-3517U CPU @ 1.90GHz, it takes about 2msec
   * to run.
   */
  srtSurface(QObject *parent=0);

  /**
   * \brief Constructs an algebraic surface
   *
   * @param equation String which will be passed on to setEquation()
   * @param parent Standard argument for the QObject constructor
   *
   * Convenience constructor which calls the default constructor then then sets
   * an equation using setEquation(). The following two code snippets are
   * equivalent.
   *
   * @code
   * ...
   * srtSurface surf("x^2+y^2-z^2-1", 1.0);
   * ...
   * @endcode
   * @code
   * ...
   * srtSurface surf;
   * surf.setEquation("x^2+y^2-z^2-1");
   * surf.setA(1.0);
   * ...
   * @endcode
   *
   * Like setEquation(), this constructor can be slow. On an Intel(R) Core(TM)
   * i7-3517U CPU @ 1.90GHz, this constructor takes about 44msec to run. In
   * settings where this is an issue, it might make sense to run setEquation()
   * in a separate thread.
   */
  srtSurface(const QString &equation, qreal a=0.0, QObject *parent=0);

  /**
   * \brief Destructor
   *
   * @note If this destructor is called while the surface is still being
   * rendered in another thread, the method will block until the rendering
   * process terminates.
   */
  ~srtSurface();

  /**
   * \brief Returns the constant a
   *
   * @returns The constant that was previously set with setA()
   *
   * @see setA()
   */
  qreal getA();

  /**
   * \brief Returns the equation set with setEquation().
   *
   * @returns The equation string that was previously set with setEquation()
   *
   * @see setEquation()
   */
  QString getEquation();

  /**
   * \brief Returns error status
   *
   * @returns True if an error condition has been set because the equation
   * string does not contain a valid polynomial. In this case, an error message
   * can then be retrieved using errorString() and a pointer to the first
   * occurrence of the error is found using errorIndex().
   */
  bool hasError();

  /**
   * \brief Checks for empty surface
   *
   * @returns True if the surface is empty.
   */
  bool isEmpty();

  /**
   * \brief Human readable error message
   *
   * @returns If an error condition has been set, this method returns a human
   * readable error message in English language that describes the first error
   * found. If no error condition has been set, an empty string is returned. 
   *
   * Use the method hasError() to check if an error condition has been set.
   */
  QString errorString();

  /**
   * \brief Pointer to first error in the equation string
   *
   * @returns If an error condition has been set, this method returns the index
   * of the first error found in the equation string. If no error condition has
   * been set, zero is returned. 
   *
   * Use the method hasError() to check if an error condition has been set.
   */
  int errorIndex();

 public slots:
  /**
   * \brief Specifies the constant a
   *
   * @param a A real number that is substituted for the constant 'a' when the
   * surface is rendered.
   *
   * If appropriate, the signal changed() will be emitted.
   *
   * @note If this methods is called while the surface is still being rendered
   * in another thread, the method will block until the rendering process
   * terminates.
   *
   * @see getA()
   */
  void setA(qreal a=0);

  /**
   * \brief Specifies the equation which defines the surface implicitly and generates machine code
   *
   * @param equation String that defines a polynomial in variables x, y and z,
   * and an additional constant a, for example instance "x^2+y^2-z^2-a". Strings
   * such as "x^y" or "sqrt(x)" are not polynomial and generate errors. If the
   * string is empty, the surface is set to "empty".
   *
   * This method sets the equation which defines the surface implicitly, parses
   * the equation and generates optimized machine code which is later used when
   * the surface is rendered. If the parameter equation is not a polynomial, an
   * error condition is set. The method hasError() can be used to check if this
   * is the case. An error message can be retrieved using errorString(). A
   * pointer to the first occurrence of the error is found using errorIndex().
   *
   * If appropriate, the signal changed() will be emitted.
   *
   * @note This method is slow. On an Intel(R) Core(TM) i7-3517U CPU @ 1.90GHz
   * this method takes about 40msec to run for a reasonably-sized equation.
   *
   * @note If this method is called while the surface is still being rendered in
   * another thread, the method will block until the rendering process
   * terminates.
   *
   * @see getEquation()
   */
  void setEquation(const QString &equation=QString::null);
 
  /**
   * \brief Emits the signal changed()
   */
  void touch() {emit changed();}
 
 signals:
  /**
   * \brief Emitted whenever any member of this class changes its value
   *
   * This signal is emitted whenever any member of this class changes its value
   */
  void changed();

 private:
  friend class srtScene;

  // Default constructor. I have implemented this as a separate, private method
  // so that more than one constructor implementation can use method --calling
  // 'default constructors' from other constructors is nearly impossible in standard
  // C++.
  void construct();

  // Mutex used to serialize access to the parser and the methods get_err_str()
#warning flawed design
  static QMutex parserSerialization;
  
  // Read-Write lock, to be used for all private members that are defined below
  // this point
  QReadWriteLock privateMemberLock;

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
  int _errorIndex;

  // Constant a
  qreal _a;

  // Pointer to parser interna. 
#warning I do not properly understand what that is.
  class parser::jit *jit;
  struct parser_tree *curve_tree;
  struct parser_tree *deriv_tree[3];


#warning this should not be left open
#warning non-qt types used here which should be hidden
  int64_t (*stripe)(struct stripe_data *sd, int j);
#warning this should not be left open
#warning non-qt types used here which should be hidden
#warning It seems that this is never used
  v4sf (*curve)(m34sf v, float a);

};

#endif
