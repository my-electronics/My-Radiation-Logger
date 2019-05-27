#ifndef MYPLOT_H
#define MYPLOT_H

#include <QWidget>

#include <QBrush>
#include <QPen>

class Tick
{

public:
    Tick();
    Tick(double value, int pixel, double span);
    int posPixel;
    double unit, span;
    QString getLabel(void);

private:
    QString label;

};


class Axis
{

public:
    Axis();
    Axis(double min, double max, bool inverted);
    double min, max;
    void resize(int size);
    QVector<Tick> majorTicks, minorTicks;
    int getPixel(double unit);


private:
    double unitsPerPixel, pixelsPerUnit, span;
    bool inverted;
    int size;
    QVector<Tick> generateTicks(int tickCount);
    void updateScale();
    void updateTicks();

};


class XYData
{

public:
    XYData();
    XYData(QVector<double> X, QVector<double> Y, QColor lineColor, double lineWidth, QColor markerColor, double markerSize, QString label);
    QVector<double> X, Y;
    double lineWidth, markerSize;
    QColor lineColor, markerColor;
    QString label;

};


class MyPlot : public QWidget
{
    Q_OBJECT

public:
    explicit MyPlot(QWidget *parent = nullptr);

    void plotXY(QVector<double> X, QVector<double> Y, QColor lineColor = Qt::blue,  double lineWidth = 1.5, QColor markerColor = Qt::blue, double markerSize = 0, QString label = "");
    void setAxes(double xmin, double xmax, double ymin, double ymax);
    void setAxesAuto();
    void setFramePen(QPen penFrame);
    void setTicksPen(QPen penTicks);
    void setXLabel(QString xLabel, int fontSize = 8);
    void setYLabel(QString yLabel, int fontSize = 8);
    void setGridPen(QPen penGrid);
    void setPadding(int padLeft, int padTop, int padRight, int padBottom);
    void clear();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int padLeft = 60, padTop = 30, padRight = 30, padBottom = 60, posB, posR;
    QColor colorGraphBackground;
    QPen penFrame = QPen(Qt::black, 1), penTicks = QPen(Qt::black, 1), penGrid = QPen(Qt::gray, 1);
    QString xLabel, yLabel;
    int xLabelFontSize, yLabelFontSize;
    Axis xAxis, yAxis;
    QVector<XYData> xyDataList;
    QVector<QPoint> pointsFromVectors(QVector<double> X, QVector<double> Y);
    QPixmap frame, graph;
    void renderFrame();
    void renderGraph();
    bool updateFrame;

};



#endif // MYPLOT_H
