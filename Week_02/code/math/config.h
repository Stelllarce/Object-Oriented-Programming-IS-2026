#pragma once
/**
 * const globals defined in a header are safe to include in multiple .cpp files.
 * Each translation unit gets its own copy – no ODR violation,
 * because const globals have internal linkage by default in C++.
 */
const int MAX_RETRIES = 5;

// -----------------------------------------------------------------
// WHY the pointer must also be const for internal linkage:
//
//   const char*  COLORS[] = { ... };   // BAD: pointer is mutable
//                                       // external linkage by default
//                                       // ODR violation if included in multiple .cpp
//
//   const char* const COLORS[] = { ... }; // GOOD: both pointer and data are const
//                                          // internal linkage, safe in headers
// -----------------------------------------------------------------
const char* const COLORS[]     = { "red", "green", "blue" };
const int         COLORS_COUNT = 3;

const char* const DIRECTIONS[]     = { "north", "south", "east", "west" };
const int         DIRECTIONS_COUNT = 4;
