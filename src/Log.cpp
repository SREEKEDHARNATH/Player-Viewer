#include "Log.hpp"
#include <iostream>
#include <stdexcept>

namespace core{
    template <typename T>void logData(const T& data, LOG l){
        if (l==LOG::DATA){
            std::cout << data << "\n";
        }
        else if (l==LOG::INFO){
            std::cout << "Info: " << data << "\n";
        }
        else if(l==LOG::WARN){
            std::cout << "Warning: " << data << "|n";
        }
        else{
            std::cerr << "Error: " << data << "\n";
            throw std::runtime_error("Aborted due to error");
        }
    
    }
    void log(int x, LOG l){
        logData<int>(x, l);
    }
    void log(float x, LOG l){
        logData<float>(x, l);
    }
    void log(double x, LOG l){
        logData<double>(x, l);
    }
    void log(long x, LOG l){
        logData<long>(x, l);
    }
    void log(long long x, LOG l){
        logData<long long>(x, l);
    }
    
    void log(const char* data, LOG l){
        logData<const char*>(data, l);
    }
    void log(std::string& data, LOG l){
        logData<std::string>(data, l);
    }
    void log(const std::string& data, LOG l){
        logData<const std::string>(data, l);
    }
}
