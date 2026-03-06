// trigger_fail.cpp
// Demonstrates how failbit is raised in different common situations.
//
// Situations covered:
//   1. Type mismatch – trying to read an int from a non-numeric token.
//   2. Reading from a stream that was never opened (empty path / bad file).
//   3. Reading after EOF (both eofbit and failbit are set).
//   4. Recovering from failbit with clear() + ignore().
//
// Data file used: ../../data/mixed.txt
// Contents:
//   42
//   hello     <- this will break an int >> read
//   99

#include <fstream>
#include <iostream>

// Helper: print the current flag state
void printFlags(const std::ifstream& in, const char* label) {
    std::cout << label
              << ": good=" << in.good()
              << " eof="   << in.eof()
              << " fail="  << in.fail()
              << " bad="   << in.bad()
              << "\n";
}

int main() {
    // --- Situation 1: type mismatch ---
    std::cout << "=== Situation 1: type mismatch ===\n";

    std::ifstream in("../../data/mixed.txt");
    if (!in) { std::cerr << "Cannot open file!\n"; return 1; }

    int x;
    in >> x;
    printFlags(in, "After reading 42");   // good
    std::cout << "  x = " << x << "\n";

    in >> x;  // tries to read "hello" into int -> failbit raised
    printFlags(in, "After reading 'hello' into int");  // fail=1, bad=0
    std::cout << "  x (unchanged) = " << x << "\n";   // x keeps its old value

    // Recovery: clear the failbit, then skip the bad token
    in.clear();
    char bad_token[64];
    in >> bad_token;    // consume the word that caused the failure
    printFlags(in, "After clear() + consuming bad token");

    in >> x;
    printFlags(in, "After reading 99");
    std::cout << "  x = " << x << "\n";

    // --- Situation 2: opening a nonexistent file ---
    std::cout << "\n=== Situation 2: nonexistent file ===\n";
    std::ifstream missing("nonexistent_file.txt");
    printFlags(missing, "After opening nonexistent file");
    // fail() is true – the open itself set failbit

    // --- Situation 3: reading after EOF ---
    std::cout << "\n=== Situation 3: reading after EOF ===\n";
    std::ifstream in2("../../data/numbers.txt");
    if (!in2) { std::cerr << "Cannot open file!\n"; return 1; }

    // drain the file
    while (in2 >> x) {}
    printFlags(in2, "After draining file (eof+fail set)");

    in2 >> x;  // another read on an exhausted stream
    printFlags(in2, "After extra read past EOF");

    return 0;
}
