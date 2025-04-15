#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
// Caminho completo para o QCustomPlot para garantir que o Docker o encontre
#include "/app/build/qcustomplot/qcustomplot.h"
#include <QDebug>

MainWindow::MainWindow(RingBuffer& buffer, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_dataBuffer(buffer) {
    ui->setupUi(this);
    
    // Set up the main widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);
    
    // Create and add the plot widget
    m_customPlot = new QCustomPlot(this);
    layout->addWidget(m_customPlot);
    
    // Configure the plot
    m_customPlot->addGraph();
    m_customPlot->graph(0)->setPen(QPen(Qt::blue));
    m_customPlot->xAxis->setLabel("Time");
    m_customPlot->yAxis->setLabel("Voltage (V)");
    m_customPlot->setInteraction(QCP::iRangeDrag, true);
    m_customPlot->setInteraction(QCP::iRangeZoom, true);
    
    // Setup timer for periodic updates
    connect(&m_updateTimer, &QTimer::timeout, this, &MainWindow::updatePlot);
    m_updateTimer.start(100); // Update every 100ms
    
    // Set window properties
    resize(800, 600);
    setWindowTitle("Board_B_12V Voltage Monitor");
}

MainWindow::~MainWindow() {
    m_updateTimer.stop();
}

void MainWindow::updatePlot() {
    // Get the latest data from the buffer
    auto [times, values] = m_dataBuffer.getData();
    
    if (values.empty()) {
        return; // No data to plot yet
    }
    
    // Update the plot with new data
    m_customPlot->graph(0)->setData(QVector<double>(times.begin(), times.end()), 
                                    QVector<double>(values.begin(), values.end()));
    
    // Auto-scale the axes if this is the first update
    if (m_firstUpdate) {
        m_customPlot->rescaleAxes();
        m_firstUpdate = false;
    }
    
    // Refresh the plot
    m_customPlot->replot();
}
