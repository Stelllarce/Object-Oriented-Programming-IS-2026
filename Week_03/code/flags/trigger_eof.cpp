// trigger_eof.cpp
// Demonstrates how eofbit is raised when a read reaches the end of the file.
//
// Key insight: eofbit is set DURING the read that encounters EOF,
// NOT before it. The last successful value is still extracted.
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

    int x;

    // --- Scenario 1: detecting EOF correctly in a loop ---
    std::cout << "=== Correct EOF loop ===\n";
    while (in >> x) {           // loop exits when >> fails (eofbit + failbit)
        std::cout << "  Read: " << x << "\n";
    }

    if (in.eof()) {
        std::cout << "Reached end of file normally.\n";
    }

    // --- Scenario 2: reading PAST eof without clear() ---
    std::cout << "\n=== Reading past EOF without clear() ===\n";
    // Both eofbit and failbit are now set; the next >> will immediately fail
    in >> x;
    std::cout << "Read attempt failed (as expected): fail()=" << in.fail() << "\n";

    // --- Scenario 3: recover and re-read from the beginning ---
    std::cout << "\n=== Recover with clear() + seekg() ===\n";
    in.clear();                       // clear eofbit and failbit
    in.seekg(0, std::ios::beg);       // rewind to the start

    std::cout << "After clear+seekg: good()=" << in.good() << "\n";

    while (in >> x) {
        std::cout << "  Re-read: " << x << "\n";
    }

    return 0;
}
