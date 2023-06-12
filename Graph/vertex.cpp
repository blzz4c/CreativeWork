#include "vertex.h"
#include "edge.h"
#include "graph.h"

#include <QVector>

Vertex::Vertex(Graph *graph, int index) : graph(graph){
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    vertexIndex = index;
    degree = 0;
    color = "white";
    name = "";
}

Vertex::~Vertex(){
    clearEdge();
    degree = 0;
}

QString Vertex::getVertexColor(){
    return this -> color;
}

QString Vertex::getVertexName(){
    return name;
}

QVector<Edge *> Vertex::getEdges() const{
    return edgeList;
}

int Vertex::getVertexDegree(){
    return degree;
}

int Vertex::getVertexIndex(){
    return vertexIndex;
}

void Vertex::setVertexColor(QString color){
    this -> color = color;
    qApp->processEvents();
    update();
}

void Vertex::setVertexName(QString name){
    this -> name = name;
}

void Vertex::setVertexIndex(int index){
    vertexIndex = index;
    qApp->processEvents();
    update();
}

void Vertex::addEdge(Edge *edge){
    edgeList << edge;
    edge -> adjust();
    degree ++;
}

void Vertex::calculateForces(){
    if(!scene() || scene()->mouseGrabberItem() == this){
        newPos = pos();
        return;
    }

    qreal xvel = 0;
    qreal yvel = 0;
    const QList<QGraphicsItem *> items = scene()->items();
    for(QGraphicsItem *item : items){
        Vertex *temp = qgraphicsitem_cast<Vertex *>(item);
        if(!temp){
            continue;
        }
        QPointF vec = mapToItem(temp, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();
        double l = 2.0 * (dx * dx + dy * dy);
        if(l > 0 && l < 600){
            xvel += (dx * 150.0) / l;
            yvel += (dy * 150.0) / l;
        }
    }
    if(qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1){
        xvel = yvel = 0;
    }

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 145), sceneRect.bottom() - 10));
}


bool Vertex::advancePosition(){
    if(newPos == pos()){
        return false;
    }
    setPos(newPos);
    return true;
}

bool Vertex::pathExist(int dest){
    for(Edge *temp : edgeList){
        if(temp -> destVertex() -> getVertexIndex() == dest){
            return true;
        }
    }
    return false;
}

void Vertex::insertEdge(int dest, double weight){
    const QList<QGraphicsItem *> items = scene()->items();
    for(QGraphicsItem *item : items){
        Vertex *temp = qgraphicsitem_cast<Vertex *>(item);
        if(!temp){
            continue;
        }
        if(temp->getVertexIndex() == dest){
            Edge *edge = new Edge(this, temp, weight);
            edgeList.append(edge);
            return;
        }
    }
}

void Vertex::updateWeight(int dest, double weight){
    for(Edge *temp : edgeList){
        if(temp -> destVertex() -> getVertexIndex() == dest){
            temp -> setEdgeWeight(weight);
        }
    }
}

void Vertex::eraseEdge(int dest){
    if(!pathExist(dest)){
        return;
    }
    for(int i{0}; i < edgeList.size(); i++){
        if(edgeList[i] -> destVertex() -> getVertexIndex() == dest){
            edgeList.erase(edgeList.begin() + i);
        }
    }
    degree --;
}


void Vertex::clearEdge(){
    edgeList.clear();
    degree = 0;
}

void Vertex::changeEdgeColor(int dest){
    for(Edge *edge : edgeList){
        if(edge->destVertex()->getVertexIndex() == dest){
            edge->setEdgeColor("black");
        }
    }
}

QRectF Vertex::boundingRect() const{
    qreal adjust = 2;
    return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Vertex::shape() const{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Vertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *){

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);

        if (this -> color == "white"){
            gradient.setColorAt(1, QColor(Qt::green).lighter(120));
            gradient.setColorAt(0, QColor(Qt::darkGreen).lighter(120));
        }
        else if (this -> color == "gray"){
            gradient.setColorAt(1, QColor(Qt::lightGray).lighter(120));
            gradient.setColorAt(0, QColor(Qt::darkGray).lighter(120));
        }
        else if (this -> color == "black"){
            gradient.setColorAt(1, QColor(Qt::cyan).lighter(120));
            gradient.setColorAt(0, QColor(Qt::darkCyan).lighter(120));
        }

    } else {
        if (this -> color == "white"){
            gradient.setColorAt(0, QColor(Qt::green).lighter(120));
            gradient.setColorAt(1, QColor(Qt::darkGreen).lighter(120));
        }
        else if (this -> color == "gray"){
            gradient.setColorAt(0, QColor(Qt::lightGray).lighter(120));
            gradient.setColorAt(1, QColor(Qt::darkGray).lighter(120));
        }
        else if (this -> color == "black"){
            gradient.setColorAt(0, QColor(Qt::cyan).lighter(120));
            gradient.setColorAt(1, QColor(Qt::darkCyan).lighter(120));
        }
    }
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
    QString index = QString::number(vertexIndex);
    painter->drawText(QRect(-10, -10, 20, 20), Qt::AlignCenter, index);
}


QVariant Vertex::itemChange(GraphicsItemChange change, const QVariant &value){
    switch(change){
        case ItemPositionHasChanged:
            for(Edge *edge : qAsConst(edgeList)){
                graph->itemMoved();
                edge->adjust();
                for(Edge *reverseEdge : edge->destVertex()->getEdges()){
                    reverseEdge->adjust();
                }
            }
            graph->itemMoved();
            break;
        default:
            break;
    };
    return QGraphicsItem::itemChange(change, value);
}


void Vertex::mousePressEvent(QGraphicsSceneMouseEvent *event){
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Vertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
