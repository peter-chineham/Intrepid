
#include "canvas.h"

//#include "arc.h"
//#include "Edge.h"
#include "linetype.h"
#include "Network.h"
//#include "Node.h"
#include "Parm.h"
#include "world.h"

Canvas *Canvas::my_canvas = NULL;
string Canvas::show_network = "all";
string Canvas::show_locations = "all";
string Canvas::show_lines = "all";
string Canvas::show_labels = "all";
string Canvas::show_type = "normal";

Canvas::Canvas()
{
    drawing = false;
    pv_width = 0;
    pv_height = 0;
    zooming = false;
    orig.setX(0);
    orig.setY(0);
    move.setX(0);
    move.setY(0);
}

QRectF Canvas::boundingRect() const
{
    //    cout << pv_width << " " << pv_height << endl;
    return QRectF(0, 0, pv_width, pv_height);
}

void Canvas::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int nn = (int) Network::nodes.size();
    int ne = (int) Network::edges.size();

	if (drawing) {
        if (show_network == "all") {
            for (int i = 0; i < ne; i++) {
                if (Network::edges[i].edgenum == -1)
                    continue; // This link has been deleted
                drawEdge(painter, i);
            }
            for (int i = 0; i < nn; i++) {
                drawNode(painter, i);
                if ((show_labels == "all")
                        || ((show_labels == "backbone") && (Network::nodes[i].ntyped != 'E'))) {
                    drawLabel(painter, i);
                }
            }
        }
        else if (show_network == "backbone") {
            for (int i = 0; i < ne; i++) {
                if (Network::edges[i].edgenum == -1)
                    continue; // This link has been deleted
                if ((Network::nodes[Network::edges[i].ends[0]].ntyped != 'E')
                        && (Network::nodes[Network::edges[i].ends[1]].ntyped != 'E')) {
                    drawEdge(painter, i);
                }
            }
            for (int i = 0; i < nn; i++) {
                if (Network::nodes[i].ntyped != 'E') {
                    drawNode(painter, i);
                    if (show_labels != "none") {
                        drawLabel(painter, i);
                    }
                }
            }
        }
        else if (show_network == "clusters") {
            for (int i = 0; i < ne; i++) {
                if (Network::edges[i].edgenum == -1)
                    continue; // This link has been deleted
                int ends0 = Network::edges[i].ends[0];
                int ends1 = Network::edges[i].ends[1];
                if ((Network::nodes[ends0].ntyped == 'E')
                        || (Network::nodes[ends1].ntyped == 'E')) {
                    drawEdge(painter, i);
                    drawNode(painter, ends0);
                    drawNode(painter, ends1);
                    if ((show_labels == "all") || ((show_labels == "backbone") 
						&& (Network::nodes[ends0].ntyped != 'E'))) {
                        drawLabel(painter, ends0);
                    }
                    if ((show_labels == "all") || ((show_labels == "backbone") 
						&& (Network::nodes[ends1].ntyped != 'E'))) {
                        drawLabel(painter, ends1);
                    }
                }
            }
        } else if (show_network == "tree") {
            for (int i = 0; i < ne; i++) {
                if (Network::edges[i].edgenum == -1)
                    continue; // This link has been deleted
                if (Network::edges[i].Ltree == true) {
                    drawEdge(painter, i);
                    int ends0 = Network::edges[i].ends[0];
                    int ends1 = Network::edges[i].ends[1];
                    drawNode(painter, ends0);
                    drawNode(painter, ends1);
                    if ((show_labels == "all") || ((show_labels == "backbone") 
						&& (Network::nodes[ends0].ntyped != 'E'))) {
                        drawLabel(painter, ends0);
                    }
                    if ((show_labels == "all") || ((show_labels == "backbone") 
						&& (Network::nodes[ends1].ntyped != 'E'))) {
                        drawLabel(painter, ends1);
                    }
                }
            }
        } else if (show_network == "direct") {
            for (int i = 0; i < ne; i++) {
                if (Network::edges[i].edgenum == -1)
                    continue; // This link has been deleted
                if (Network::edges[i].Ltree == false) {
                    drawEdge(painter, i);
                    int ends0 = Network::edges[i].ends[0];
                    int ends1 = Network::edges[i].ends[1];
                    drawNode(painter, ends0);
                    drawNode(painter, ends1);
                    if ((show_labels == "all") || ((show_labels == "backbone") 
						&& (Network::nodes[ends0].ntyped != 'E'))) {
                        drawLabel(painter, ends0);
                    }
                    if ((show_labels == "all") || ((show_labels == "backbone") 
						&& (Network::nodes[ends1].ntyped != 'E'))) {
                        drawLabel(painter, ends1);
                    }
                }
            }
        }
        if (zooming) {
            QPen  zpen(Qt::gray);
            zpen.setWidth(1);
            QRect zrect(orig.x(), orig.y(),
                        abs(move.x()-orig.x()),
                        abs(move.y()-orig.y()));
            QBrush zbrush(Qt::NoBrush);
            painter->setPen(zpen);
            painter->setBrush(zbrush);
            painter->drawRect(zrect);
        }
    }
}

void Canvas::drawEdge(QPainter *painter, int l)
{
    int n1 = Network::edges[l].ends[0];
    int n2 = Network::edges[l].ends[1];
    float x1 = Network::nodes[n1].screen.x();
    float y1 = Network::nodes[n1].screen.y();
    float x2 = Network::nodes[n2].screen.x();
    float y2 = Network::nodes[n2].screen.y();
    float util;
    QColor col;
    int arc0, arc1;

    if (show_type == "normal") {
        col = Network::edges[l].Lcolor;
    } else {
        arc0 = Network::edges[l].arcs[0];
        arc1 = Network::edges[l].arcs[1];
        if (arc0 != -1) {
            if (Parm::getParm("DUPLEXITY") == "HALF") {
                util = Network::arcs[arc0].utilization + Network::arcs[arc1].utilization;
            } else {
                util = max(Network::arcs[arc0].utilization, Network::arcs[arc1].utilization);
            }
        } else {
            // Might be TNDA Mentor
            if (Network::edges[l].load != -1)
                util = Network::edges[l].load / (Linetype::pLinetype[Network::edges[l].type].speedin * Network::edges[l].mult);
            else
                return; // don't try and process this edge!
        }
        if (util <= 0) {
            col = Network::edge_colours[0];
        } else if (util <= 0.1) {
            col = Network::edge_colours[1];
        } else if (util <= 0.2) {
            col = Network::edge_colours[2];
        } else if (util <= 0.3) {
            col = Network::edge_colours[3];
        } else if (util <= 0.4) {
            col = Network::edge_colours[4];
        } else if (util <= 0.5) {
            col = Network::edge_colours[5];
        } else if (util <= 0.6) {
            col = Network::edge_colours[6];
        } else if (util <= 0.7) {
            col = Network::edge_colours[7];
        } else if (util <= 0.8) {
            col = Network::edge_colours[8];
        } else if (util <= 0.9) {
            col = Network::edge_colours[9];
        } else if (util <= 1.0) {
            col = Network::edge_colours[10];
        } else {
            col = Network::edge_colours[11];
        }
    }
    //    cout << col << endl;
    int mult = Network::edges[l].mult;

    QPen linepen(col);
    linepen.setWidth(mult);

    QPoint p1;
    p1.setX(x1);
    p1.setY(y1);

    QPoint p2;
    p2.setX(x2);
    p2.setY(y2);

    painter->setPen(linepen);
    painter->drawLine(p1, p2);
}

void Canvas::drawNode(QPainter *painter, int n)
{
	QString name = QString::fromStdString(Network::nodes[n].name);
	char  ntype = Network::nodes[n].ntyped;
	float x = Network::nodes[n].screen.x();
	float y = Network::nodes[n].screen.y();

	QRect  rec(x - 1, y - 1, 3, 3);
	QBrush brush(Qt::SolidPattern);

	switch (ntype) {
	case 'B': // Both
		brush.setColor(Qt::blue);
		break;
	case 'C': // Centre
		brush.setColor(Qt::red);
		break;
	case 'E': // End
		brush.setColor(Qt::green);
		break;
	case 'N': // Network
		brush.setColor(Qt::magenta);
		break;
	}
	painter->setBrush(brush);

	// draw a filled circle using a path
	QPainterPath epath;
	epath.addEllipse(rec);           // defines the area to be filled
	painter->fillPath(epath, brush); // fill it with settings for brush
}

void Canvas::drawLabel(QPainter *painter, int n)
{
    QString name = QString::fromStdString(Network::nodes[n].name);
    char  ntype = Network::nodes[n].ntyped;
    float x = Network::nodes[n].screen.x();
    float y = Network::nodes[n].screen.y();

    QPen pen(Qt::black);
    switch(ntype) {
    case 'B' : // Both
        pen.setColor(Qt::blue);
        break;
    case 'C' : // Centre
        pen.setColor(Qt::red);
        break;
    case 'E' : // End
        pen.setColor(Qt::green);
        break;
    case 'N' : // Network
        pen.setColor(Qt::magenta);
        break;
    }
    painter->setPen(pen);

    // draw label
    painter->drawText((int) x, (int) y, name);
}

void Canvas::drawNetwork()
{
    drawing = true;
    update();
}

void Canvas::clearNetwork()
{
    drawing = false;
}

void Canvas::zoomExtents()
{
    int minx = 108000;
    int maxx = -108000;
    int miny = 108000;
    int maxy = -108000;
    int nn = (int) Network::nodes.size();
	for (int i = 0; i < nn; i++) {
        if (Parm::getParm("LLVH") == "VH") {
            if (Network::nodes[i].hcord < minx) {
                minx = Network::nodes[i].hcord;
            }
            if (Network::nodes[i].hcord > maxx) {
                maxx = Network::nodes[i].hcord;
            }
            if (Network::nodes[i].vcord < miny) {
                miny = Network::nodes[i].vcord;
            }
            if (Network::nodes[i].vcord > maxy) {
                maxy = Network::nodes[i].vcord;
            }
        } else if (Parm::getParm("LLVH") == "LL") {
            if (Network::nodes[i].longitude < minx) {
                minx = Network::nodes[i].longitude;
            }
            if (Network::nodes[i].longitude > maxx) {
                maxx = Network::nodes[i].longitude;
            }
            if (Network::nodes[i].latitude < miny) {
                miny = Network::nodes[i].latitude;
            }
            if (Network::nodes[i].latitude > maxy) {
                maxy = Network::nodes[i].latitude;
            }
        }
    }
    minx -= 100;
    maxx += 100;
    miny -= 100;
    maxy += 100;
    World::viewport[0].setX(minx);
    World::viewport[0].setY(miny);
    World::viewport[1].setX(maxx);
    World::viewport[1].setY(maxy);
//    cout << "zoomExtents minx:" << minx << " maxx:" << maxx << " miny:" << miny << " maxy:" << maxy << endl;
//    cout << "zoomExtents pv_width:" << my_canvas->pv_width << " pv_height:" << my_canvas->pv_height << endl;
    for (int i = 0; i < nn; i++) {
        if (Parm::getParm("LLVH") == "VH") {
			Network::nodes[i].screen = World::ConvertWorldtoScreen(Network::nodes[i].hcord,
				Network::nodes[i].vcord, my_canvas->pv_width, my_canvas->pv_height);
        }
        else if (Parm::getParm("LLVH") == "LL") {
			Network::nodes[i].screen = World::ConvertWorldtoScreen(Network::nodes[i].longitude,
				Network::nodes[i].latitude, my_canvas->pv_width, my_canvas->pv_height);
        }
//        cout << "Node:" << i << " x:" << Network::nodes[i].screen.x() << " y:" << Network::nodes[i].screen.y() << endl;
    }
}

void Canvas::setCanvas(Canvas *canvas)
{
    my_canvas = canvas;
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (drawing) {
        if (event->button() == Qt::LeftButton) {
            zooming = true;
            orig = event->pos();
            move = event->pos();
            update();
        } else if (event->button() == Qt::RightButton) {
            zoomExtents();
            update();
        }
    }
}

void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ((drawing == true) && (zooming == true)) {
        move = event->pos();
        update();
    }
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int nn = (int) Network::nodes.size();

	if ((drawing == true) && (zooming == true)) {
        zooming = false;

        // Check didn't just click canvas
        QPointF XY = event->pos();
        if ((orig.x() == XY.x()) || (orig.y() == XY.y())) {
            return;
        }
        QPoint HoldXY1 = World::ConvertScreentoWorld(orig.x(), orig.y(), pv_width, pv_height);
        QPoint HoldXY2 = World::ConvertScreentoWorld(XY.x(), XY.y(), pv_width, pv_height);

        // Allow box to be drawn any way round you like!
        if (HoldXY1.x() < HoldXY2.x()) {
            World::viewport[0].setX(HoldXY1.x());
            World::viewport[1].setX(HoldXY2.x());
        } else {
            World::viewport[0].setX(HoldXY2.x());
            World::viewport[1].setX(HoldXY1.x());
        }
        if (HoldXY1.y() < HoldXY2.y()) {
            World::viewport[0].setY(HoldXY1.y());
            World::viewport[1].setY(HoldXY2.y());
        } else {
            World::viewport[0].setY(HoldXY2.y());
            World::viewport[1].setY(HoldXY1.y());
        }

        for (int i = 0; i < nn; i++) {
            if (Parm::getParm("LLVH") == "VH") {
				Network::nodes[i].screen = World::ConvertWorldtoScreen(
					Network::nodes[i].hcord, Network::nodes[i].vcord, 
					pv_width, pv_height);
            }
            else if (Parm::getParm("LLVH") == "LL") {
				Network::nodes[i].screen = World::ConvertWorldtoScreen(
					Network::nodes[i].longitude, Network::nodes[i].latitude,
					pv_width, pv_height);
            }
        }
        update();
    }
}
