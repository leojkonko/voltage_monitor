#include <QApplication>
#include <QThread>
#include <memory>
#include "mainwindow.h"
#include "main.cpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Check command line arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <json_file_path>" << std::endl;
        return 1;
    }
    
    // Set up the data buffer
    std::shared_ptr<RingBuffer> buffer = std::make_shared<RingBuffer>();
    
    // Create the data reader
    std::string filePath = argv[1];
    DataReader reader(filePath, *buffer);
    
    // Read the data
    reader.readAllData();
    
    // Create a thread to simulate live data
    QThread* dataThread = new QThread();
    QObject* dataFeeder = new QObject();
    dataFeeder->moveToThread(dataThread);
    
    // Connect the thread start signal to start simulating live data
    QObject::connect(dataThread, &QThread::started, [&reader, dataFeeder]() {
        reader.simulateLiveData(100);  // 100ms delay between updates
        // Note: simulateLiveData has an infinite loop, so this lambda will not return
    });
    
    // Set up deletion when thread finishes
    QObject::connect(dataThread, &QThread::finished, dataThread, &QThread::deleteLater);
    QObject::connect(dataThread, &QThread::finished, dataFeeder, &QObject::deleteLater);
    
    // Create and show the main window
    MainWindow window(*buffer);
    window.show();
    
    // Start the data simulation thread
    dataThread->start();
    
    // Run the application event loop
    int result = app.exec();
    
    // Clean up
    dataThread->quit();
    dataThread->wait();
    
    return result;
}
