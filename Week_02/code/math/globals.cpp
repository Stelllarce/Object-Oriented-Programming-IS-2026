/**
 * Demonstrates internal linkage for mutable globals via anonymous namespace.
 * 'counter' and 'incrementCounter' are invisible to every other translation unit.
 */
namespace {
    int counter = 0;

    void incrementCounter() {
        counter++;
    }
}

// A public function that uses the internal state without exposing it.
void doWork() {
    incrementCounter();
    // counter is accessible here, but main.cpp cannot see it at all.
}
