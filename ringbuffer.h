#pragma once

#include <vector>
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>

class RingBuffer {
public:
    explicit RingBuffer(size_t capacity = 1000)
        : m_capacity(capacity), m_buffer(), m_timeBuffer() {}

    void push(double value, double time) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_buffer.size() >= m_capacity) {
            m_buffer.pop_front();
            m_timeBuffer.pop_front();
        }
        
        m_buffer.push_back(value);
        m_timeBuffer.push_back(time);
        
        m_cv.notify_one();
    }

    std::pair<std::vector<double>, std::vector<double>> getData() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<double> data(m_buffer.begin(), m_buffer.end());
        std::vector<double> times(m_timeBuffer.begin(), m_timeBuffer.end());
        return {times, data};
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_buffer.size();
    }

    bool waitForData(const std::chrono::milliseconds& timeout) {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_cv.wait_for(lock, timeout, [this] { return !m_buffer.empty(); });
    }

private:
    size_t m_capacity;
    std::deque<double> m_buffer;
    std::deque<double> m_timeBuffer;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};
