#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include "/app/build/qcustomplot/qcustomplot.h"
#include <QDebug>

MainWindow::MainWindow(RingBuffer& buffer, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_dataBuffer(buffer) {
    ui->setupUi(this);
    
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);
    
    m_customPlot = new QCustomPlot(this);
    layout->addWidget(m_customPlot);
    
    m_customPlot->addGraph();
    m_customPlot->graph(0)->setPen(QPen(Qt::blue));
    m_customPlot->xAxis->setLabel("Time");
    m_customPlot->yAxis->setLabel("Voltage (V)");
    m_customPlot->setInteraction(QCP::iRangeDrag, true);
    m_customPlot->setInteraction(QCP::iRangeZoom, true);
    
    connect(&m_updateTimer, &QTimer::timeout, this, &MainWindow::updatePlot);
    m_updateTimer.start(100);
    
    resize(800, 600);
    setWindowTitle("Board_B_12V Voltage Monitor");
}

MainWindow::~MainWindow() {
    m_updateTimer.stop();
}

void MainWindow::updatePlot() {
    auto [times, values] = m_dataBuffer.getData();
    
    if (values.empty()) {
        return;
    }
    
    m_customPlot->graph(0)->setData(QVector<double>(times.begin(), times.end()), 
                                    QVector<double>(values.begin(), values.end()));
    
    if (m_firstUpdate) {
        m_customPlot->rescaleAxes();
        m_firstUpdate = false;
    }
    
    m_customPlot->replot();
}
