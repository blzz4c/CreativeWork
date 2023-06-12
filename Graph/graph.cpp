#include "graph.h"
#include <QTextStream>
#include <QMessageBox>
#include <QGridLayout>
#include "priority_queue.h"

bool flag=false;
QString kommiOutput;

Graph::Graph(QWidget *parent) : QGraphicsView(parent){
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-910, -400, 1600, 900);

    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(1.1), qreal(1.1));
    setMinimumSize(1920, 1080);

    connected = true;
    DFS.clear();
    BFS.clear();
}

void Graph::itemMoved(){
    if(!timerId){
        timerId = startTimer(1000 / 25);
    }
}

void Graph::timerEvent(QTimerEvent *event){
    Q_UNUSED(event);
    const QList<QGraphicsItem *> items = scene -> items();
    for(Vertex *temp : qAsConst(vertexList)){
        temp->calculateForces();
    }
    bool itemsMoved = false;
    for(Vertex *temp : qAsConst(vertexList)){
        if(temp->advancePosition()){
            itemsMoved = true;
        }
    }

    if(!itemsMoved){
        killTimer(timerId);
        timerId = 0;
    }
}

void Graph::drawBackground(QPainter *painter, const QRectF &rect){
    Q_UNUSED(rect);

    QRectF sceneRect = this->sceneRect();

    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::green);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);
    QRectF textRect(sceneRect.left() + sceneRect.width(), sceneRect.top() + sceneRect.height()/9 + 50, sceneRect.width() - 4, 20);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(20);
    painter->setFont(font);
    painter->setPen(Qt::green);
    createTabWidget(rect);
}

void Graph::createTabWidget(const QRectF &rect){
    Q_UNUSED(rect);
    QRectF sceneRect = this -> sceneRect();

    QTabWidget *tab = new QTabWidget();
    tab->setGeometry(sceneRect.left(), sceneRect.top(), sceneRect.width()/6, sceneRect.height());
    QWidget *buildTab = new QWidget;
    QWidget *graphTab = new QWidget;

    tab->setAttribute(Qt::WA_StyledBackground);
    QFont textfont = QFont();
    textfont.setPointSize(12);
    tab -> setFont(textfont);
    tab->setIconSize(QSize(20, 25));

    tab->addTab(buildTab, QIcon(":/images/icon"), tr("Построение"));
    tab->addTab(graphTab, QIcon(":/images/icon"), tr("Граф"));

    QGridLayout *gridBuild = new QGridLayout();

    QVBoxLayout *buildTabLayout = new QVBoxLayout;

    QPushButton *addEdgeButton = new QPushButton();
    gridBuild->addWidget(addEdgeButton, 0, 0);
    addEdgeButton->setText(tr("Добавить\nдугу"));
    addEdgeButton->setFont(textfont);
    addEdgeButton->setMaximumSize(QSize(150, 2000));
    addEdgeButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                                 "QPushButton:pressed{background-color: lightGreen;} "
                                 "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(addEdgeButton, &QPushButton::clicked, this, &Graph::createAddEdgeWindow);

    buildTabLayout->addWidget(addEdgeButton);

    QPushButton *updateWeightButton = new QPushButton();
    gridBuild->addWidget(updateWeightButton, 1, 0);
    updateWeightButton->setText(tr("Обновить\nвес"));
    updateWeightButton->setFont(textfont);
    updateWeightButton->setMaximumSize(QSize(150, 2000));
    updateWeightButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                                  "QPushButton:pressed{background-color: lightGreen;} "
                                  "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(updateWeightButton, &QPushButton::clicked, this, &Graph::createUpdateWeightWindow);

    buildTabLayout->addWidget(updateWeightButton);

    QPushButton *eraseEdgeButton = new QPushButton();
    gridBuild->addWidget(eraseEdgeButton, 2, 0);
    eraseEdgeButton->setText(tr("Удалить\nдугу"));
    eraseEdgeButton->setFont(textfont);
    eraseEdgeButton->setMaximumSize(QSize(150, 2000));
    eraseEdgeButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                                 "QPushButton:pressed{background-color: lightGreen;} "
                                 "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(eraseEdgeButton, &QPushButton::clicked, this, &Graph::createEraseEdgeWindow);

    buildTabLayout->addWidget(eraseEdgeButton);

    QPushButton *addVertexButton = new QPushButton();
    gridBuild->addWidget(addVertexButton, 0, 1);
    addVertexButton->setText(tr("Добавить\nвершину"));
    addVertexButton->setFont(textfont);
    addVertexButton->setMaximumSize(QSize(150, 2000));
    addVertexButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                                   "QPushButton:pressed{background-color: lightGreen;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(addVertexButton, &QPushButton::clicked, this, &Graph::insertVertex);

    buildTabLayout->addWidget(addVertexButton);

    QPushButton *eraseVertexButton = new QPushButton();
    gridBuild->addWidget(eraseVertexButton, 1, 1);
    eraseVertexButton->setText(tr("Удалить\nвершину"));
    eraseVertexButton->setFont(textfont);
    eraseVertexButton->setMaximumSize(QSize(150, 2000));
    eraseVertexButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                                   "QPushButton:pressed{background-color: lightGreen;} "
                                   "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");

    connect(eraseVertexButton, &QPushButton::clicked, this, &Graph::createEraseVertexWindow);

    buildTabLayout->addWidget(eraseVertexButton);

    QGridLayout *gridGraph = new QGridLayout();

    QVBoxLayout *graphTabLayout = new QVBoxLayout;

    QToolButton *dfsButton = new QToolButton();
    gridGraph->addWidget(dfsButton, 0, 0);
    dfsButton->setText(tr("Поиск в\nглубину"));
    dfsButton->setFont(textfont);
    dfsButton->setMaximumSize(QSize(150, 2000));
    dfsButton->setStyleSheet("QToolButton:selected, QToolButton:hover{color: darkGreen;} "
                             "QToolButton:pressed{background-color: lightGreen;} "
                             "QToolButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(dfsButton, &QPushButton::clicked, this, &Graph::createDepth_First_SearchWindow);

    graphTabLayout->addWidget(dfsButton);

    QToolButton *bfsButton = new QToolButton();
    gridGraph->addWidget(bfsButton, 1, 0);
    bfsButton->setText(tr("Поиск в\nширину"));
    bfsButton->setFont(textfont);
    bfsButton->setMaximumSize(QSize(150, 2000));
    bfsButton->setStyleSheet("QToolButton:selected, QToolButton:hover{color: darkGreen;} "
                             "QToolButton:pressed{background-color: lightGreen;} "
                             "QToolButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(bfsButton, &QPushButton::clicked, this, &Graph::createBreadth_First_SearchWindow);
    graphTabLayout->addWidget(bfsButton);

    QToolButton *dijkstraButton = new QToolButton();
    gridGraph->addWidget(dijkstraButton, 2, 0);
    dijkstraButton->setText(tr("Алгоритм\nДейкстры"));
    dijkstraButton->setFont(textfont);
    dijkstraButton->setMaximumSize(QSize(150, 2000));
    dijkstraButton->setStyleSheet("QToolButton:selected, QToolButton:hover{color: darkGreen;} "
                             "QToolButton:pressed{background-color: lightGreen;} "
                             "QToolButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(dijkstraButton, &QPushButton::clicked, this, &Graph::createDijkstraWindow);
    graphTabLayout->addWidget(dijkstraButton);

    QToolButton *kommiButton = new QToolButton();
    gridGraph->addWidget(kommiButton, 3, 0);
    kommiButton->setText(tr("Задача\nКоммивояжера"));
    kommiButton->setFont(textfont);
    kommiButton->setMaximumSize(QSize(150, 2000));
    kommiButton->setStyleSheet("QToolButton:selected, QToolButton:hover{color: darkGreen;} "
                             "QToolButton:pressed{background-color: lightGreen;} "
                             "QToolButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(kommiButton, &QPushButton::clicked, this, &Graph::kommi);

    QPushButton *resetButton = new QPushButton();
    gridGraph->addWidget(resetButton, 0, 1);
    resetButton->setText(tr("Обновить\nграф"));
    resetButton->setFont(textfont);
    resetButton->setMaximumSize(QSize(150, 2000));
    resetButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                             "QPushButton:pressed{background-color: lightGreen;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(resetButton, &QPushButton::clicked, this, &Graph::reset);
    graphTabLayout->addWidget(resetButton);

    QPushButton *clearButton = new QPushButton();
    gridGraph->addWidget(clearButton, 1, 1);
    clearButton->setText(tr("Удалить\nграф"));
    clearButton->setFont(textfont);
    clearButton->setMaximumSize(QSize(150, 2000));
    clearButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                             "QPushButton:pressed{background-color: lightGreen;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(clearButton, &QPushButton::clicked, this, &Graph::clear);
    graphTabLayout->addWidget(clearButton);

    QPushButton *graphInfoButton = new QPushButton();
    gridGraph->addWidget(graphInfoButton, 2, 1);
    graphInfoButton->setText(tr("Информация\nо графе"));
    graphInfoButton->setFont(textfont);
    graphInfoButton->setMaximumSize(QSize(150, 2000));
    graphInfoButton->setStyleSheet("QPushButton:selected, QPushButton:hover{color: darkGreen;} "
                             "QPushButton:pressed{background-color: lightGreen;} "
                             "QPushButton{height: 30px; width: 30px; background-color: lightGray; border-right, border-bottom: 4px solid Gray;}");
    connect(graphInfoButton, &QPushButton::clicked, this, &Graph::getGraphAbout);
    graphTabLayout->addWidget(graphInfoButton);

    buildTab->setLayout(gridBuild);
    graphTab->setLayout(gridGraph);

    scene->addWidget(tab);

}

int Graph::getVertexNum(){
    return vertexList.size();
}

void Graph::scaleView(qreal scaleFactor){
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < 0.07 || factor > 100){
        return;
    }
    scale(scaleFactor, scaleFactor);
}

void Graph::insertVertex(){
    int vertexIndex = vertexList.size();
    vertexList.append(new Vertex(this, vertexIndex));
    vertexList[vertexIndex] -> setPos(0, 50);
    scene->addItem(vertexList[vertexIndex]);
    vertexNum++;
}

void Graph::updateIndex(){
    for(int i{0}; i < vertexList.size(); i++){
        vertexList[i] -> setVertexIndex(i);
    }
}

void Graph::eraseVertex(int vertexIndex){
    Vertex *temp = vertexList[vertexIndex];
    for(Edge *edge : temp->getEdges()){
        eraseEdge(vertexIndex, edge->destVertex()->getVertexIndex());
    }
    temp->clearEdge();
    scene->removeItem(temp);
    edgeNum -= vertexList[vertexIndex]->getEdges().size()*2;
    vertexList.erase(vertexList.begin() + vertexIndex);
    updateIndex();
    vertexNum--;
}

void Graph::getGraphAbout(){
    int rows = vertexList.size() + 5;
    int cols = vertexList.size() + 1;
    QTableWidget *infoWindow = new QTableWidget(rows, cols);

    QString dijkstra1 = "Алгоритм Дейкстры по индексу ";
    if(flag==true && dijkstraSignal()!=-1)
    {
        QVector<int> k=dijkstra(dijkstraSignal());
        dijkstra1+= QString::number(dijkstraSignal());
        for(int i{0}; i < k.size(); i++){
            infoWindow->setItem(0, i+1, new QTableWidgetItem(QString::number(k[i])));
        }
    }

    infoWindow->setItem(0, 0, new QTableWidgetItem(dijkstra1));

    QString dfs = "Поиск в глубину по индексу ";
    if(!DFS.empty()){
        dfs += QString::number(DFS[0]);
        for(int i{0}; i < DFS.size(); i++){
            infoWindow->setItem(1, i+1, new QTableWidgetItem(QString::number(DFS[i])));
        }
    }
    infoWindow->setItem(1, 0, new QTableWidgetItem(dfs));

    QString bfs = "Поиск в ширину по индексу ";
    if(!BFS.empty()){
        bfs += QString::number(BFS[0]);
        for(int i{0}; i < BFS.size(); i++){
            infoWindow->setItem(2, i+1, new QTableWidgetItem(QString::number(BFS[i])));
        }
    }
    infoWindow->setItem(2, 0, new QTableWidgetItem(bfs));

    infoWindow->setItem(4, 0, new QTableWidgetItem("Матрица графа"));
    infoWindow->item(4, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    for(int i{0}; i < vertexNum; i++){
        infoWindow->setItem(4, i+1, new QTableWidgetItem(QString::number(i)));
        infoWindow->item(4, i+1)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    for(int i{0}; i < vertexNum; i++){
        infoWindow->setItem(5+i, 0, new QTableWidgetItem(QString::number(i)));
        infoWindow->item(5+i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        for(int j{0}; j < vertexNum; j++){
            infoWindow->setItem(5+i, 1+j, new QTableWidgetItem("0"));
            infoWindow->item(5+i, 1+j)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        for(Edge *edge : vertexList[i]->getEdges()){
            int pos = edge->destVertex()->getVertexIndex();
            infoWindow->setItem(5+i, 1+pos, new QTableWidgetItem(QString::number(edge->getEdgeWeight())));
            infoWindow->item(5+i, 1+pos)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }
        infoWindow->setColumnWidth(i+1, 30);
    }
    infoWindow->setWindowTitle("Информационное окно графа");
    infoWindow->horizontalHeader()->setVisible(false);
    infoWindow->verticalHeader()->setVisible(false);
    infoWindow->setColumnWidth(0, 200);
    infoWindow->setFixedSize(QSize(600, 700));
    infoWindow->show();
}

void Graph::insertEdge(int source, int dest, double weight){
    if(checkAdjacent(source, dest)){
        return;
    } else {
        scene->addItem(new Edge(vertexList[source], vertexList[dest], weight));
        scene->addItem(new Edge(vertexList[dest], vertexList[source], weight));
        edgeNum += 2;
    }
}

void Graph::updateWeight(int source, int dest, double weight){
    if(!checkAdjacent(source, dest)){
        return;
    }
    vertexList[source]->updateWeight(dest, weight);
    vertexList[dest]->updateWeight(source, weight);
}

void Graph::eraseEdge(int source, int dest){
    if(!checkAdjacent(source, dest)){
        scene->addText(tr("не связан"));
        return;
    }
    for(Edge *edge : vertexList[source]->getEdges()){
        if(edge -> destVertex() ->getVertexIndex() == dest){
            vertexList[source]->eraseEdge(dest);
            scene->removeItem(edge);
        }
    }
    for(Edge *edge : vertexList[dest]->getEdges()){
        if(edge -> destVertex() ->getVertexIndex() == source){
            vertexList[dest]->eraseEdge(source);
            scene->removeItem(edge);
        }
    }
    edgeNum -= 2;
}

bool Graph::checkAdjacent(int source, int dest){
    return vertexList[source] -> pathExist(dest);
}

int Graph::getDegree(int vertexIndex){
    return vertexList[vertexIndex]->getVertexDegree();
}

void Graph::clear(){
    while(!vertexList.empty()){
        eraseVertex(0);
    }
}

void Graph::checkConnected(){
    connected = true;
    for(Vertex *vertex : vertexList){
        bool visited = false;
        if(vertex->getVertexColor() == "black"){
            visited = true;
        }
        connected &= visited;
    }
}

void Graph::reset(){
    if(vertexList.size() == 0){
        return;
    }
    for(int i{0}; i < vertexList.size(); i++){
        vertexList[i] -> setVertexColor("white");
        for(Edge *edge : vertexList[i]->getEdges()){
            edge -> setEdgeColor("white");
        }
    }
}

void Graph::DepthInit(int vertexIndex, QVector<bool> &visited){
    QStack<int> dfsStack;
    dfsStack.push(vertexIndex);
    while(!dfsStack.empty()){
        int temp = dfsStack.top();
        Vertex *tempVertex = vertexList[temp];
        dfsStack.pop();
        if(!visited[temp]){
            visited[temp] = true;
            vertexList[temp]->setVertexColor("black");
            DFS.push_back(temp);
            delay(50);
        }
        for(Edge *tempEdge : tempVertex->getEdges()){
            if(!visited[tempEdge -> destVertex() -> getVertexIndex()]){
                tempEdge->destVertex()->setVertexColor("lightGray");
                delay(70);
                dfsStack.push(tempEdge->destVertex()->getVertexIndex());
            }
        }
    }
}

void Graph::Depth_First_Search(int vertexIndex){
    DFS.clear();
    QVector<bool> visited(vertexList.size(), false);
    DepthInit(vertexIndex, visited);
    checkConnected();
}

void Graph::BreadthInit(int vertexIndex, QVector<bool> &visited){
    QQueue<int> bfsQueue;
    bfsQueue.push_back(vertexIndex);
    while(!bfsQueue.empty()){
        int temp = bfsQueue.front();

        Vertex *tempVertex = vertexList[temp];
        bfsQueue.pop_front();
        if(!visited[temp]){
            visited[temp] = true;
            tempVertex->setVertexColor("black");
            BFS.push_back(temp);
            delay(50);
        }
        for(Edge *tempEdge : tempVertex->getEdges()){
            if(!visited[tempEdge -> destVertex() -> getVertexIndex()]){
                tempEdge->destVertex()->setVertexColor("lightGray");
                bfsQueue.push_back(tempEdge->destVertex()->getVertexIndex());
                delay(70);
            }
        }
    }
}

void Graph::Breadth_First_Search(int vertexIndex){
    BFS.clear();
    QVector<bool> visited(vertexList.size(), false);
    BreadthInit(vertexIndex, visited);
    checkConnected();
}

QVector<int> Graph::dijkstra(int vertexIndex){
    int Adj_matrix[vertexNum][vertexNum];
    for(int i=0;i<vertexNum;i++){
        for(int j=0;j<vertexNum;j++){
            if(i==j) Adj_matrix[i][j] = 0;
            else Adj_matrix[i][j] = 100000000;
        }
    }
    PriorityQueue<int> queue;
    for(int i=0;i<vertexNum;i++){
        queue.enqueue(Adj_matrix[vertexIndex][i],i);
    }
    while(queue.count()>0){
        int Curr_vertex = queue.dequeue();
        int edge_count = vertexList[Curr_vertex]->getEdges().count();
        for(int i=0;i<edge_count;i++){
            Edge* e = vertexList[Curr_vertex]->getEdges()[i];
            Vertex* dest = e->destVertex();
            if(Adj_matrix[vertexIndex][Curr_vertex]+e->getEdgeWeight()<Adj_matrix[vertexIndex][dest->getVertexIndex()]){
                int old = Adj_matrix[vertexIndex][dest->getVertexIndex()];
                Adj_matrix[vertexIndex][dest->getVertexIndex()] = Adj_matrix[vertexIndex][Curr_vertex]+e->getEdgeWeight();
                queue.update(old,dest->getVertexIndex(),Adj_matrix[vertexIndex][dest->getVertexIndex()]);
            }
        }
    }
    QVector<int> new_vector;
    for(int i=0;i<vertexNum;i++){
        new_vector.append(Adj_matrix[vertexIndex][i]);
    }
    return new_vector;
}



void Graph::kommi() {
    int **Adj_matrix = new int*[vertexNum] {};
    int ** origAdj_matrix = new int*[vertexNum] {};
    for (int i = 0; i < vertexNum; i++) {
        Adj_matrix[i] = new int[vertexNum]{};
        origAdj_matrix[i] = new int[vertexNum]{};
    }
    for (Vertex* i : vertexList) {
            for (Edge* j: i->getEdges()) {
                Adj_matrix[i->getVertexIndex()][j->destVertex()->getVertexIndex()] = j->getEdgeWeight();
                origAdj_matrix[i->getVertexIndex()][j->destVertex()->getVertexIndex()] = j->getEdgeWeight();
            }
    }
    bool ToContinue = (vertexNum > 2);
    for (int i = 0; i < vertexNum && ToContinue; i++) {
        ToContinue = (vertexList[i]->getEdges().size() > 1);
    }
    if (ToContinue) {
        int Sum = 0, m = 10000000;
        for (int i = 0; i < vertexNum; i++) {
            for (int j = 0; j < vertexNum; j++) {
                if (Adj_matrix[i][j] == 0) {
                    Adj_matrix[i][j] = m;
                }
            }
        }
        QMap<int,int> roads = {};
        for (int z = 0; z < vertexNum; z++) {
            int maxi = 0, maxj = 0;
            int di[vertexNum], dj[vertexNum];
            int maxScore = 0;
            for (int i = 0; i < vertexNum; i++) {
                di[i] = dj[i] = m;
            }
            for (int i = 0; i < vertexNum; i++) {
                for (int j = 0; j < vertexNum; j++) {
                    if (Adj_matrix[i][j] < di[i]) {
                        di[i] = Adj_matrix[i][j];
                    }
                }
            }
            for (int i = 0; i < vertexNum; i++) {
                for (int j = 0; j < vertexNum; j++) {
                        if (Adj_matrix[i][j] != m) {
                            Adj_matrix[i][j] -= di[i];
                         }
                }
            }
            for (int i = 0; i < vertexNum; i++) {
                for (int j = 0; j < vertexNum; j++) {
                    if (Adj_matrix[j][i] < dj[i]) {
                        dj[i] = Adj_matrix[j][i];
                    }
                }
            }
            for (int i = 0; i < vertexNum; i++) {
                for (int j = 0; j < vertexNum; j++) {
                    if (Adj_matrix[j][i] != m) {
                        Adj_matrix[j][i] -= dj[i];
                    }
                }
            }
            for (int i = 0; i < vertexNum; i++) {
                for (int j = 0; j < vertexNum; j++) {
                    int imin = m, jmin = m;
                    if (Adj_matrix[i][j] == 0) {
                        for (int k = 0; k < vertexNum; k++) {
                            if (Adj_matrix[k][j] < imin && i != k) {
                                imin = Adj_matrix[k][j];
                            }
                            if (Adj_matrix[i][k] < jmin) {
                                jmin = Adj_matrix[i][k];
                            }
                        }
                        if (maxScore < imin + jmin) {
                            maxScore = imin + jmin;
                            maxi = i;
                            maxj = j;
                        }
                    }
                }
            }
            roads[maxj] = maxi;
            Adj_matrix[maxj][maxi] = m;
            Sum += origAdj_matrix[maxi][maxj];

            for (int i = 0; i < vertexNum; i++) {
                Adj_matrix[i][maxj] = m;
                Adj_matrix[maxi][i] = m;
            }
        }

        int i = 0;
        kommiOutput += QString::number(i);
        while(ToContinue) {
            kommiOutput += "-> " + QString::number(roads[i]);
            i = roads[i];
            ToContinue = (i != 0);
        }
        kommiOutput += " Сумма = " + QString::number(Sum);
    }
    else {
        kommiOutput += "Решение задачи Коммивояжёра невозможно!\nУ каждой вершины графа должны быть как минимум два ребра!";
    }
    for (int i = 0; i < vertexNum; i++) {
        delete[] Adj_matrix[i];
        delete[] origAdj_matrix[i];
    }
    delete[] Adj_matrix;
    delete[] origAdj_matrix;
    createKommiWindow();
}

void Graph::delay(int time) {
    clock_t now = clock();
    while(clock() - now < time);
}

void Graph::createEraseVertexWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Стереть индекс вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно стирания вершины");

    connect(okButton, &QPushButton::clicked, this, &Graph::eraseVertexSignal);
    window->show();
}

void Graph::createAddEdgeWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Индекс исходной вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Индекс конечной вершины:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Вес дуги:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно добавления дуги");

    connect(okButton, &QPushButton::clicked, this, &Graph::addEdgeSignal);
    window->show();
}

void Graph::createEraseEdgeWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Индекс исходной вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Индекс конечной вершины:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(okButton, 2, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно удаления дуги");

    connect(okButton, &QPushButton::clicked, this, &Graph::eraseEdgeSignal);
    window->show();
}

void Graph::createUpdateWeightWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    input2 = new QLineEdit;
    input3 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Индекс исходной вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(new QLabel(tr("Индекс конечной вершины:")), 1, 0);
    layout->addWidget(input2, 1, 1);
    layout->addWidget(new QLabel(tr("Вес дуги:")), 2, 0);
    layout->addWidget(input3, 2, 1);
    layout->addWidget(okButton, 3, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно обновления веса");

    connect(okButton, &QPushButton::clicked, this, &Graph::updateWeightSignal);
    window->show();
}

void Graph::createDepth_First_SearchWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Начальный индекс вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно ПвГ");

    connect(okButton, &QPushButton::clicked, this, &Graph::depthSignal);
    window->show();
}

void Graph::createBreadth_First_SearchWindow(){
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Начальный индекс вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно ПвШ");

    connect(okButton, &QPushButton::clicked, this, &Graph::breadthSignal);
    window->show();
}

void Graph::createDijkstraWindow()
{
    window = new QWidget;
    input1 = new QLineEdit;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);
    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(tr("Начальный индекс вершины:")), 0, 0);
    layout->addWidget(input1, 0, 1);
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно алгоритма Дейкстры");

    connect(okButton, &QPushButton::clicked, this, &Graph::dijkstraSignal);
    window->show();
}

void Graph::createKommiWindow()
{
    window = new QWidget;
    QLabel *label = new QLabel;
    label->setFrameStyle(QFrame::Box | QFrame::Plain);

    QPushButton *okButton = new QPushButton(tr("OK"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new QLabel(kommiOutput), 0, 1);
    kommiOutput = "";
    layout->addWidget(okButton, 1, 1, Qt::AlignRight);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    window->setLayout(layout);
    window->setWindowTitle("Окно Коммивояжера");

    connect(okButton, &QPushButton::clicked, this, &Graph::kommiSignal);
    window->show();
}

void Graph::eraseVertexSignal(){
    int srcIndex = input1->text().toInt();
    window -> close();
    if(srcIndex >= vertexList.size()){
        return;
    }
    eraseVertex(srcIndex);
}

void Graph::addEdgeSignal(){
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    double edgeWeight = input3->text().toDouble();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size() || edgeWeight <= 0){
        return;
    }
    insertEdge(srcIndex, destIndex, edgeWeight);
}

void Graph::eraseEdgeSignal(){
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size()){
        return;
    }
    eraseEdge(srcIndex, destIndex);
}

void Graph::updateWeightSignal(){
    int srcIndex = input1->text().toInt();
    int destIndex = input2->text().toInt();
    double edgeWeight = input3->text().toDouble();
    window->close();
    if(srcIndex >= vertexList.size() || destIndex >= vertexList.size() || edgeWeight <= 0){
        return;
    }
    updateWeight(srcIndex, destIndex, edgeWeight);
}

void Graph::depthSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    Depth_First_Search(index);
}

void Graph::breadthSignal(){
    int index = input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return;
    }
    Breadth_First_Search(index);
}

int Graph::dijkstraSignal()
{
    flag=true;
    int index=input1->text().toInt();
    window->close();
    if(index >= vertexList.size()){
        return -1;
    }
    return index;
}

void Graph::kommiSignal(){
    window->close();
}
