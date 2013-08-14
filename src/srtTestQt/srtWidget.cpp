#include <QDebug>
#include <QPainter>

#include "srtWidget.h"


srtWidget::srtWidget(QWidget *parent)
  : QFrame(parent)
{
  scene.surface.setEquation("4*((a*(1+sqrt(5))/2)^2*x^2-1*y^2)*((a*(1+sqrt(5))/2)^2*y^2-1*z^2)*((a*(1+sqrt(5))/2)^2*z^2-1*x^2)-1*(1+2*(a*(1+sqrt(5))/2))*(x^2+y^2+z^2-1*1)^2");
}


srtWidget::~srtWidget()
{
}


void srtWidget::paintEvent(QPaintEvent *event)
{
  QPainter painter(this);
  
#warning libsrt can only draw square images. This is a temporary workaround. Fix this.
  painter.fillRect( QRectF(frameRect().left()+frameWidth(),
			   frameRect().top()+frameWidth(), 
			   frameRect().width()-2*frameWidth(),
			   frameRect().height()-2*frameWidth() ), Qt::black);

  int size = qMin( frameRect().width()-2*frameWidth(), frameRect().height()-2*frameWidth() );
  QImage img = scene.draw( QSize(size, size) );
  painter.drawImage( QPoint(frameWidth() + ((frameRect().width()-2*frameWidth())-size)/2  , frameWidth() + ((frameRect().height()-2*frameWidth())-size)/2), img);
  
  painter.end();
  QFrame::paintEvent(event);
}
