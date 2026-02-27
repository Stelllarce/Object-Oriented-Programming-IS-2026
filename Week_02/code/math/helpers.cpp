/**
 * 'static' at file scope is the traditional C-style way to get internal linkage.
 * It is equivalent to wrapping the symbol in an anonymous namespace.
 * Prefer anonymous namespaces in modern C++.
 */

static int helperCount = 0;  // Internal linkage – not visible outside this file

static void internalHelper() {
    helperCount++;  // Implementation detail – inaccessible from any other .cpp
}
