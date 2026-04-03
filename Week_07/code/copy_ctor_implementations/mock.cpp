#include "mock.hpp"

Mock::Mock(size_t s) : size(s), data(new int[n]{})
Mock::~Mock() { delete[] dyn_data; }


Mock(const Mock& other) {
    
}
Mock& operator=(const Mock& other);