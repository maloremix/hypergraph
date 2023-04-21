#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QVector>
#include <QPointF>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <QSet>
#include <QMap>
#include <cmath>
#include <random>
#include <QDebug>
enum class Margin
{
    Left             = 0,
    Top              = 1,
    Right            = 2,
    Bottom           = 3,
    None             = 4
};

struct Element
{
    int index;
    QString name,
            type;
    int portsCount;
    QPoint pos;
    QVector<QPair<int,Margin>> ports;
    bool placed;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    srand(static_cast<unsigned int>(time(nullptr))); // seed for random number generator

    int numElements = 1000;
    int rows = qRound(sqrt(numElements))*1.5;
    int maxPortsCount = 6;
    int circCount = 30;
    int dx = 110;
    int dy = 110;
    int v = 30;
    int portsNum;
    int circInd;
    int N = 10;
    int mod = circCount/N;

    QVector<int> circVect;
    QMap<QString, std::shared_ptr<Element>> elementsMap;

    for(int nodeInd=0; nodeInd < numElements; ++nodeInd)
    {
        portsNum = 0;
        while(!portsNum) portsNum = rand() % maxPortsCount;

        Element el;
        el.name = QString("%1").arg(nodeInd);
        el.index = nodeInd;
        el.portsCount = portsNum;
        el.placed = false;
        el.pos = QPoint((nodeInd % rows)*dx + ((nodeInd / rows)%2 ? 5 : 0) - (rand()%2 ? -v : v),
                        (nodeInd / rows)*dy + ((nodeInd % rows)%2 ? 5 : 0) - (rand()%2 ? -v : v));

        int de = nodeInd/(numElements/N);

        QSet<int> uniqueCircuits;
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::normal_distribution<double> distribution(circCount/2, circCount/6); // задаем параметры нормального распределения

        for(int portInd=0; portInd < portsNum; ++portInd)
        {
            double circIndDouble = distribution(generator); // генерируем индекс цепи по нормальному распределению
            int circInd = qBound(0, static_cast<int>(std::round(circIndDouble)), circCount-1); // округляем и ограничиваем индекс в диапазоне [0, circCount-1]

            qDebug() << circInd;

            while(uniqueCircuits.contains(circInd))
            {
                circIndDouble = distribution(generator);
                circInd = qBound(0, static_cast<int>(std::round(circIndDouble)), circCount-1);
            }
            uniqueCircuits.insert(circInd);

            Margin portMargin = static_cast<Margin>(rand()%4);
            el.ports.push_back(QPair<int, Margin>(circInd, portMargin));

            if(!circVect.count(circInd)) circVect.push_back(circInd);
        }
        elementsMap.insert(QString("%1").arg(nodeInd), std::make_shared<Element>(el));
    }

    // создаем графическую сцену
    QGraphicsScene scene;
    scene.setSceneRect(0, 0, rows*dx + dx/2, ((numElements-1) / rows + 1)*dy + dy/2);

    // добавляем вершины на сцену
    QMapIterator<QString, std::shared_ptr<Element>> it(elementsMap);
    while (it.hasNext()) {
        it.next();
        auto el = it.value();
        QGraphicsRectItem* item = new QGraphicsRectItem(el->pos.x()-5, el->pos.y()-5, 10, 10);
        scene.addItem(item);
    }

//    // отрисовываем связи между узлами
//    for (auto it = elementsMap.begin(); it != elementsMap.end(); ++it) {
//        auto el = it.value();

//        for (auto& port : el->ports) {
//            int cIndex = port.first;
//            auto elConnected = elementsMap.values(QString::number(port.first))[0];

//            QGraphicsLineItem* line = new QGraphicsLineItem(el->pos.x(), el->pos.y(),
//                                                            elConnected->pos.x(), elConnected->pos.y());
//            scene.addItem(line);
//        }
//    }
    // создаем окно приложения
    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(QBrush(QColor(255, 255, 255)));
    view.setWindowTitle("Random Graph");
    view.show();

    // запускаем приложение
    return app.exec();
}

