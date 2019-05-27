#include "myplot.h"

#include <QPainter>

#include <QDebug>

Tick::Tick()
{

}

Tick::Tick(double value, int pixel, double span)
{
    this->unit = value;
    this->posPixel = pixel;
    this->span = span;
}

QString Tick::getLabel(void)
{
    QString label;
    if(span < .01) return label.sprintf("%.4f", unit);
    if(span < .1) return label.sprintf("%.3f", unit);
    if(span < 1) return label.sprintf("%.2f", unit);
    if(span < 10) return label.sprintf("%.1f", unit);

    return label.sprintf("%.0f", unit);
}

Axis::Axis()
{

}

Axis::Axis(double min, double max, bool inverted = false)
{
    this->min = min;
    this->max = max;
    this->inverted = inverted;
    resize(100);
}

void Axis::resize(int size)
{
    this->size = size;
    updateScale();
}

void Axis::updateScale()
{
    pixelsPerUnit = double(size / (max - min));
    unitsPerPixel = 1. / pixelsPerUnit;
    updateTicks();
}

int Axis::getPixel(double unit)
{
    int pixel = int(((unit - min) * pixelsPerUnit));
    if (inverted)
        pixel = size - pixel;
    return pixel;
}

double findTickSize(double targetTickSize)
{
    int lastDivision = 2;
    double round = 1000000000000;
    while (round > 0.00000000001)
    {
        if (round <= targetTickSize) return round;
        round /= lastDivision;
        if (lastDivision == 2) lastDivision = 5;
        else lastDivision = 2;
    }
    return 0;
}

QVector<Tick> Axis::generateTicks(int targetTickCount)
{
    QVector<Tick> ticks;

    //ticks.append(Tick(min, getPixel(min), max - min));

    if (targetTickCount > 0)
    {

        double tickSize = findTickSize(((max - min) / targetTickCount) * 1.5);
        int lastTick = 0; // !?
        for (int i = 0; i < size; i++)
        {
            double pos = i * unitsPerPixel + min;
            int thisTick = int((pos / tickSize));
            if (thisTick != lastTick)
            {
                lastTick = thisTick;
                double posRounded = double((int(pos / tickSize) * tickSize));
                if ((posRounded >= min) && (posRounded <= max))
                {
                    ticks.append(Tick(posRounded, getPixel(posRounded), max - min));
                }
            }
        }
    }

    //ticks.append(Tick(max, getPixel(max), max - min));

    return ticks;
}

void Axis::updateTicks()
{
    double pixelsPerTick = 80;
    int targetTickCount = int(size / pixelsPerTick);
    majorTicks = generateTicks(targetTickCount);
}

XYData::XYData() {}

XYData::XYData(QVector<double> X, QVector<double> Y, QColor lineColor, double lineWidth, QColor markerColor, double markerSize, QString label)
{
    this->X = X;
    this->Y = Y;
    this->lineWidth = lineWidth;
    this->lineColor = lineColor;
    this->markerSize = markerSize;
    this->markerColor = markerColor;
    this->label = label;

}

MyPlot::MyPlot(QWidget *parent) : QWidget(parent)
{
    xAxis = Axis(-10, 10);
    yAxis = Axis(-10, 10, true);

    xLabel = "X";
    yLabel = "Y";
}


void MyPlot::setPadding(int padLeft, int padTop, int padRight, int padBottom)
{
    this->padLeft = padLeft;
    this->padTop = padTop;
    this->padRight = padRight;
    this->padBottom = padBottom;
    updateFrame = true;
    update();
}

void MyPlot::setAxes(double xmin, double xmax, double ymin, double ymax)
{
    xAxis.min = xmin;
    xAxis.max = xmax;
    yAxis.min = ymin;
    yAxis.max = ymax;
    updateFrame = true;
    update();
}

void MyPlot::setAxesAuto()
{
    double xmin = 0, xmax = 0, ymin = 0, ymax = 0;

    for (QVector<XYData>::iterator xyData = xyDataList.begin(); xyData != xyDataList.end(); xyData++)
    {
        if (int(xmin) == int(xmax)) // the first data we are scaling to
        {
            xmin = *std::min_element(xyData->X.begin(), xyData->X.end());
            xmax = *std::max_element(xyData->X.begin(), xyData->X.end());
            ymin = *std::min_element(xyData->Y.begin(), xyData->Y.end());
            ymax = *std::max_element(xyData->Y.begin(), xyData->Y.end());
        }
        else // we have seen some data before, so only rescale if it expands the axes
        {
            xmin = qMin(xmin, *std::min_element(xyData->X.begin(), xyData->X.end()));
            xmax = qMax(xmax, *std::max_element(xyData->X.begin(), xyData->X.end()));
            ymin = qMin(ymin, *std::min_element(xyData->Y.begin(), xyData->Y.end()));
            ymax = qMax(ymax, *std::max_element(xyData->Y.begin(), xyData->Y.end()));
        }
    }
    setAxes(xmin, xmax, ymin, ymax);
}

void MyPlot::setFramePen(QPen penFrame)
{
    this->penFrame = penFrame;
}

void MyPlot::setTicksPen(QPen penTicks)
{
    this->penTicks = penTicks;
}

void MyPlot::setXLabel(QString xLabel, int fontSize)
{
    this->xLabel = xLabel;
    this->xLabelFontSize = fontSize;
    update();
}

void MyPlot::setYLabel(QString yLabel, int fontSize)
{
    this->yLabel = yLabel;
    this->yLabelFontSize = fontSize;
    update();
}

void MyPlot::setGridPen(QPen penGrid)
{
    this->penGrid = penGrid;
    update();
}

void MyPlot::plotXY(QVector<double> X, QVector<double> Y, QColor lineColor, double lineWidth, QColor markerColor, double markerSize, QString label)
{
    xyDataList.append(XYData(X, Y, lineColor, lineWidth, markerColor, markerSize, label));
    updateFrame = false;
    update();
}

void MyPlot::clear()
{
    xyDataList.clear();
    updateFrame = false;
    update();
}


QVector<QPoint> MyPlot::pointsFromVectors(QVector<double> X, QVector<double> Y)
{
    QVector<QPoint> points;
    for(int i = 0; i < X.length(); i++)
    {
        points.append(QPoint(xAxis.getPixel(X[i]), yAxis.getPixel(Y[i])));
    }
    return points;
}


void MyPlot::renderFrame()
{
    frame.fill(Qt::white);

    posB = graph.height() + padTop;
    posR = graph.width() + padLeft;
    int posXcenter = graph.width() / 2 + padLeft;
    int posYcenter = graph.height() / 2 + padTop;

    QPainter p(&frame);
    QFont font = p.font();
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    p.setPen(penFrame);
    p.setBrush(Qt::white);
    p.drawRect(QRect(padLeft, padTop, graph.width(), graph.height()));
    // Draw the ticks
    p.setFont(QFont(font.defaultFamily(), 8, QFont::Normal));

    for (QVector<Tick>::iterator tick = xAxis.majorTicks.begin(); tick != xAxis.majorTicks.end(); tick++)
    {
        p.setPen(penTicks);
        p.drawLine(padLeft + tick->posPixel, posB, padLeft + tick->posPixel, posB - 5);
        p.drawText(QRect(tick->posPixel + padLeft - 25, posB - 25 + 10, 50, 50), Qt::AlignVCenter | Qt::AlignHCenter, tick->getLabel(), nullptr);
        p.drawLine(padLeft + tick->posPixel, padTop, padLeft + tick->posPixel, padTop + 5);
        if (penGrid != Qt::NoPen)
        {
            p.setPen(penGrid);
            p.drawLine(padLeft + tick->posPixel, posB, padLeft + tick->posPixel, padTop);
        }
    }
    p.setFont(QFont(font.defaultFamily(), 8, QFont::Normal));
    for (QVector<Tick>::iterator tick = yAxis.majorTicks.begin(); tick != yAxis.majorTicks.end(); tick++)
    {
        p.setPen(penTicks);
        p.drawLine(padLeft, padTop + tick->posPixel, padLeft + 5, padTop + tick->posPixel);
        p.drawText(QRect(padLeft - 50 - 5, padTop + tick->posPixel - 25, 50, 50), Qt::AlignVCenter | Qt::AlignRight, tick->getLabel(), nullptr);
        p.drawLine(posR, padTop + tick->posPixel, posR - 5, padTop + tick->posPixel);
        if (penGrid != Qt::NoPen)
        {
            p.setPen(penGrid);
            p.drawLine(padLeft + 5, padTop + tick->posPixel, posR, padTop + tick->posPixel);
        }

    }

    p.setPen(Qt::black);
    // Draw the axes labels
    p.setFont(QFont(font.defaultFamily(), this->xLabelFontSize, QFont::Normal));
    p.drawText(QRect(posXcenter - 125, posB - 25 + 40, 250, 50), Qt::AlignVCenter | Qt::AlignHCenter, xLabel, nullptr);
    p.setFont(QFont(font.defaultFamily(), this->yLabelFontSize, QFont::Normal));
    p.rotate(-90);
    p.drawText(QRect(-posYcenter - 125, padLeft - 25 - 40, 250, 50), Qt::AlignVCenter | Qt::AlignHCenter, yLabel, nullptr);
}


void MyPlot::renderGraph()
{
    graph.fill(Qt::transparent);

    QPainter p(&graph);
    p.setRenderHints(QPainter::Antialiasing);

    p.setPen(Qt::blue);
    p.setBrush(Qt::blue);
    for(QVector<XYData>::iterator xyData = xyDataList.begin(); xyData != xyDataList.end(); xyData++)
    {
        QVector<QPoint> points = pointsFromVectors(xyData->X, xyData->Y);

        // draw lines
        p.setPen(QPen(xyData->lineColor, xyData->lineWidth));
        for(int i = 0; i < points.length() - 1; i++)
        {
            p.drawLine(points[i].x(), points[i].y(), points[i+1].x(), points[i+1].y());
        }
        // draw markers
        if (xyData->markerSize > 0)
        {
            int markersize = int(xyData->markerSize);
            p.setPen(Qt::NoPen);
            p.setBrush(xyData->markerColor);
            for(int i = 0; i < points.length(); i++)
            {
                p.drawEllipse(points[i].x() - markersize / 2, points[i].y() - markersize / 2, markersize, markersize);
            }
        }
    }
}


void MyPlot::paintEvent(QPaintEvent * /* event */)
{
    if (updateFrame)
    {
        int w = width(), h = height();

        // make sure the graph area is at least 1 px by 1 px
        if (width() - padLeft - padRight < 1)
            w = padLeft + padRight + 1;
        if (height() - padTop - padBottom < 1)
            h = padTop + padBottom + 1;

        frame = QPixmap(w, h);
        graph = QPixmap(frame.width() - padLeft - padRight, frame.height() - padTop - padBottom);

        xAxis.resize(graph.width());
        yAxis.resize(graph.height());

        renderFrame();
    }
    updateFrame = true;

    renderGraph();

    QPainter p(this);

    p.drawPixmap(0, 0, frame);
    p.drawPixmap(padLeft, padRight, graph);
}
