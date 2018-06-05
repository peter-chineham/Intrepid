#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QRubberBand>

using namespace std;

class Canvas : public QGraphicsItem
{
public:
    Canvas();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawEdge(QPainter *painter, int l);
    void drawNode(QPainter *painter, int n);
    void drawLabel(QPainter *painter, int n);
    void drawNetwork();
    void clearNetwork();
    void zoomExtents();

    static Canvas *my_canvas;
    static void setCanvas(Canvas *canvas);

    bool    drawing;
    qreal   pv_width;
    qreal   pv_height;
    bool    zooming;
    QPointF orig;
    QPointF move;

    static string show_network;
    static string show_locations;
    static string show_lines;
    static string show_labels;
    static string show_type;
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // CANVAS_H
