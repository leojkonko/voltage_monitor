#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>
#include "ringbuffer.h"

using json = nlohmann::json;

class DataReader {
public:
    DataReader(const std::string& filePath, RingBuffer& buffer)
        : m_filePath(filePath), m_buffer(buffer) {}

    void readAllData() {
        try {
            std::ifstream file(m_filePath);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << m_filePath << std::endl;
                return;
            }

            json data;
            file >> data;
            file.close();

            if (!data.contains("board_b_12v") || !data["board_b_12v"].contains("value")) {
                std::cerr << "Parameter 'board_b_12v.value' not found in JSON file" << std::endl;
                return;            }

            auto& values = data["board_b_12v"]["value"];
            
            std::vector<double> times;
            if (data["board_b_12v"].contains("time") && !data["board_b_12v"]["time"].empty()) {
                auto& timesJson = data["board_b_12v"]["time"];
                for (const auto& t : timesJson) {
                    times.push_back(t.get<double>());
                }
            } else {
                for (size_t i = 0; i < values.size(); ++i) {
                    times.push_back(static_cast<double>(i));
                }
            }

            std::cout << "Found " << values.size() << " values for 'board_b_12v'" << std::endl;
            m_values.clear();
            for (const auto& val : values) {
                m_values.push_back(std::stod(val.get<std::string>()));
            }
            m_times = times;
        } catch (const std::exception& e) {
            std::cerr << "Error reading JSON: " << e.what() << std::endl;
        }
    }    void simulateLiveData(int msDelay = 100) {
        if (m_values.empty()) {
            std::cerr << "No data to simulate" << std::endl;
            return;
        }

        std::cout << "Starting live data simulation..." << std::endl;
        
        size_t index = 0;
        while (true) {
            if (index >= m_values.size()) {
                index = 0;
                std::cout << "Restarting data feed simulation" << std::endl;
            }

            m_buffer.push(m_values[index], m_times[index % m_times.size()]);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(msDelay));
            
            index++;
        }
    }

private:
    std::string m_filePath;
    RingBuffer& m_buffer;
    std::vector<double> m_values;
    std::vector<double> m_times;
};

// This main function would be used when building the C++ app alone
// The actual Qt application will have its own main
#ifndef QT_MAIN
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <json_file_path>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    RingBuffer buffer;
    DataReader reader(filePath, buffer);
    
    // Read all data from the file
    reader.readAllData();
    
    // Simulate live data feed
    reader.simulateLiveData();
    
    return 0;
}
#endif
