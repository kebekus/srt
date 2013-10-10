
#ifndef COORDIANTEWIDGET_H
#define COORDIANTEWIDGET_H

#include <QGLWidget>

class CoordinateWidget : public QGLWidget
{
    Q_OBJECT

public:
    CoordinateWidget(QWidget *parent = 0);
    ~CoordinateWidget();

public slots:
    void setAxis(QVector3D newX, QVector3D newY, QVector3D newZ) {
      _newX = newX;
      _newY = newY;
      _newZ = newZ;
      update();
    };

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);


private:
    GLuint makeLine(QVector3D A, QVector3D B, qreal radius);

    void drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz, GLdouble angle);

    QList<GLuint> objects;

    QVector3D _newX;
    QVector3D _newY;
    QVector3D _newZ;
    
    QPoint lastPos;
};

#endif
