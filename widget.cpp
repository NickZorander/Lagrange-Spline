#include "widget.h"
#include <QApplication>
#include<QPainter>
#include<QPaintEvent>
#include<QtWidgets>
#include<cstdlib>
#include <QtDebug>



void MyWidget::paintEvent(QPaintEvent* e){

    QPainter painter(this);
    int i;

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black));

    QVector<QPointF>::iterator it=SplinePoints.begin();
    painter.drawPolyline(it, SplinePoints.size());

    painter.setPen(QPen(Qt::yellow, 6));
    painter.drawPoint(cursor);

    for (int i=0; i<=N-1; ++i)
    {
        painter.drawPoint(StartPoints[i]);
    }
}





void MyWidget::mouseReleaseEvent(QMouseEvent* pe)
{
    QPointF Q(pe->x(),pe->y());

    if (change_point_flag)
        {
            StartPoints[change_point_index]=Q;
            BuildSpline();
            repaint();
        }
    change_point_flag=false;
}



qreal distance (const QPointF& p1, const QPointF& p2)
{
    return qSqrt(  (p2.x()-p1.x())*(p2.x()-p1.x()) + (p2.y()-p1.y())*(p2.y()-p1.y()) );
}





void MyWidget::mousePressEvent(QMouseEvent *pe)
{
    change_point_flag=false;

    QPointF Q(pe->x(),pe->y());


    for (int i=0; i<=N-1; ++i)
        {
            if (distance(Q, StartPoints[i]) <=7)
                {
                    change_point_index=i;
                    change_point_flag=true;
                    break;
                }
        }
}


void MyWidget::mouseMoveEvent(QMouseEvent *pe)
{
    cursor=QPointF(pe->x(), pe->y());
    repaint();
}





void MyWidget::InitData()
{
    QFile inputfile("D:\\QT\\Bezier_Curve\\points.txt");

    if (!inputfile.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::information(this,"Error", "path not found!");
        return;
    } //проверили на открытие

    QTextStream stream(&inputfile);

    QString A =stream.readAll(); //сделали содержимое файла строчкой

    QStringList list = A.split(' ', QString::SkipEmptyParts); //распилили строку в список где каждый элемент - одна координата точки
    bool ok;

    N=list[0].toInt(&ok); //первым числом передается число точек

    StartPoints.resize(N);
    SplinePoints.resize(1000);


    for (int i=0; i<=N-1; ++i)
        {
            QPointF p(list[i*2+1].toInt(&ok), list[i*2+2].toInt(&ok));
            StartPoints[i]=p;
        } //создали массив точек


    change_point_flag=false;


}


//______________________________________________________________________________________________________________________________________________________________________________


bool operator<(const QPointF& p1, const QPointF& p2 )
{
      return (p1.x()<p2.x());
}


qreal MyWidget::LagrangePolynom (qreal x)
{
    double pol = 0.0;

    for (int i = 0; i <= N-1; ++i)
    {
        double basic_pol = 1;

        for (int j = 0; j <= N-1; ++j)
            if (i != j)
                basic_pol *= (x - StartPoints[j].x()) / (StartPoints[i].x() - StartPoints[j].x());

        pol += basic_pol * StartPoints[i].y();
    }
    return pol;
}



void MyWidget::BuildSpline()
{
    SplinePointsIterator=0;
    qSort(StartPoints);
    qreal x_start=StartPoints[0].x();
    qreal x_end=StartPoints[N-1].x();


    for (qreal t=0; t<=1; t+=0.001)
        {
            SplinePoints[SplinePointsIterator]=QPointF(x_start+t*(x_end-x_start), LagrangePolynom(x_start+t*(x_end-x_start)));
             ++SplinePointsIterator;
        }
}
