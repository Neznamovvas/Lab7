#ifndef THREADSAFECOUT_H
#define THREADSAFECOUT_H

#include <mutex>
#include <iostream>

namespace ThreadSafe {
    
    static inline std::mutex coutMutex;
    
    
    class Cout {
    private:
        std::lock_guard<std::mutex> lock;
        
    public:
        Cout() : lock(coutMutex) {}
        
        
        template<typename T>
        Cout& operator<<(const T& value) {
            std::cout << value;
            return *this;
        }
        
        
        Cout& operator<<(std::ostream& (*manip)(std::ostream&)) {
            std::cout << manip;
            return *this;
        }
    };
}


#define SAFE_COUT ThreadSafe::Cout()

#endif 