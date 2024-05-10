#pragma once
#include <chrono>

#include <iostream>

class Timer {
public:
    Timer(std::string name) {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
        this->name = name;
    }
    ~Timer() {
        Stop();
    }

    void Stop() {

        auto endTimepoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
        auto stop = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        auto duration = stop - start;
        double ms = duration * 0.001;
        if (stopped == false) {
            std::cout << "Timer " << name << " is:" << ms << " in milliseconds" << "\n";
            stopped = true;
        }

    }
private:
    bool stopped = false;
    std::string name;
    std::chrono::time_point <std::chrono::high_resolution_clock> m_StartTimePoint;
};