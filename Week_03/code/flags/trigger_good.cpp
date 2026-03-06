// trigger_good.cpp
// Demonstrates the good() state: the stream is fully healthy,
// all reads succeed, and no flags are raised during normal operation.
//
// Data file used: ../../data/numbers.txt
// Contents:
//   10
//   20
//   30
//   40
//   50

#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("../../data/numbers.txt");

    if (!in) {
        std::cerr << "Cannot open file!\n";
        return 1;
    }

    // Before any read, the stream should be in a good state
    std::cout << "Before reading:\n";
    std::cout << "  good() = " << in.good() << "\n"; // 1
    std::cout << "  eof()  = " << in.eof()  << "\n"; // 0
    std::cout << "  fail() = " << in.fail() << "\n"; // 0
    std::cout << "  bad()  = " << in.bad()  << "\n"; // 0

    int x;
    while (in >> x) {
        // good() is true for every iteration where the read succeeded
        std::cout << "Read: " << x
                  << "  |  good()=" << in.good()
                  << "  eof()="  << in.eof()
                  << "  fail()=" << in.fail()
                  << "  bad()="  << in.bad()
                  << "\n";
    }

    // After reading the last number the stream hits EOF.
    // good() is now false because eofbit is set.
    std::cout << "\nAfter reading all numbers:\n";
    std::cout << "  good() = " << in.good() << "\n"; // 0 – eofbit raised
    std::cout << "  eof()  = " << in.eof()  << "\n"; // 1
    std::cout << "  fail() = " << in.fail() << "\n"; // 1 – reading past EOF also sets failbit
    std::cout << "  bad()  = " << in.bad()  << "\n"; // 0

    return 0;
}
