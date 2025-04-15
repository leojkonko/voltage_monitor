#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>
#include "ringbuffer.h"

// Forward declarations
namespace Ui {
class MainWindow;
}
class QCustomPlot;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(RingBuffer& buffer, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updatePlot();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QCustomPlot *m_customPlot;
    RingBuffer& m_dataBuffer;
    QTimer m_updateTimer;
    bool m_firstUpdate = true;
};
