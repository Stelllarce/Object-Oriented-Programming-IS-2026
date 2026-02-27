# Разделна компилация, Области на видимост, Namespaces, Наредба в паметта, Padding, Bit fields, public/private

---

## Съдържание

1. [Разделна компилация](#разделна-компилация)
2. [Разположение на член-данни в паметта на структури](#разположение-на-член-данни-в-паметта-на-структури)
3. [Допълнително оптимизиране на член данни в паметта](#допълнително-оптимизиране-на-член-данни-в-паметта)
4. [`public` и `private` в структури](#public-и-private-в-структури)
5. [Гетъри и сетъри](#гетъри-и-сетъри-accessorsmutators)
6. [Задачи](#задачи)

---

## Този документ е много подробен и може да бъде overwhelming за някои от вас (което е нормално, не се притеснявайте). Можете да прегледате [RECAP.md](RECAP.md), където всичко е по-сбито и след това да се върнете и да разгледате този документ.

## Разделна компилация

При по-големи проекти е добра практика кодът да се разделя на отделни файлове. Процесът на компилация минава през няколко стъпки:

![Build process](https://faculty.cs.niu.edu/~mcmahon/CS241/Notes/Images/build.png)

### Translation unit и Linking

Компилаторът компилира всеки `.cpp` файл в отделен **translation unit** (TU) – машинен код, който съдържа дефинициите от този файл и всички включени header файлове. След това **линкера** събира всички TU и ги обединява в един изпълним файл. Ако дадена функция или глобална променлива е дефинирана в повече от един TU,линкера ще сигнализира за грешка (multiple definition). В случая ще ни бъде по-важно да разглеждаме за глобални променливи.

![Translation unit example](https://advoop.sdds.ca/assets/images/compile_link-34029fc124e56e275b31d5a1fa19424a.png)

Има два вида свързаност:
-   **Вътрешна свързаност (internal linkage)**: Символът е видим само в рамките на текущия TU. Дефинира се с `static` или чрез `const` глобални променливи (също може да се използва `constexpr`).
-   **Външна свързаност (external linkage)**: Символът е видим в други TU. Това е по подразбиране за функции и глобални променливи, освен ако не са `static` или `const`.

### Малко за препроцесор

Препроцесорът е първата стъпка от компилацията. Той обработва директивите, които започват с `#`:

```cpp
#include <iostream> // Importing from the standard library (angle brackets mean system header)
#include "math_utils.h" // Importing our own header (quotes mean user header)
#define TEST 0 // Macros - simple text substitution, substitutes every occurrence of TEST with 0 before compilation
#ifndef TEST // Conditional compilation - the code between #ifndef and #endif is included only if TEST is not defined
#endif
#pragma // Compiler-specific instructions, e.g. #pragma once for include guards
```

### Именувани пространства (Namespaces)

Предотвратяват конфликти на имена при работа с множество библиотеки:

```cpp
namespace Geometry {
    struct Point { double x, y; };
    double distance(Point a, Point b);
}

namespace Physics {
    struct Point { double x, y, z; };  // Different Point, no conflict
}

Geometry::Point p1 = {1.0, 2.0};
Physics::Point  p2 = {1.0, 2.0, 3.0};
```

### `using` директиви:
```cpp
using namespace std;     // Imports everything from std, not very good because it creates ambiguities when names clash

// This is now possible:
cout << "Hello"; // No need for std::cout

using Geometry::Point;   // Imports only Point from Geometry

// This is now possible:
Point p = {0, 0}; // No need for Geometry::Point

// Namespace alias can be useful for convenience when there are several nested namespaces:

namespace Company {
    namespace Project {
        namespace Module {
            void doWork();
        }
    }
}

// Instead of writing:
Company::Project::Module::doWork();

// We can create an alias:
namespace CPM = Company::Project::Module;
CPM::doWork();
```

**Анонимен namespace** – еквивалент на `static` за файлов обхват - всички дефиниции в анонимен namespace имат вътрешна свързаност (internal linkage) и са видими само в текущия файл:
```cpp
namespace {
    int secret = 42; // Visible only in this translation unit / file
}
```

### Декларация vs Дефиниция

-   **Декларация**: Казва на компилатора, че дадено нещо *съществува* и какъв е неговият тип/сигнатура. Не заделя памет (с изключение на `extern` промелниви).
-   **Дефиниция**: Дава пълното описание (тяло на функция, стойност на променлива). Заделя памет.

```cpp
// Function declaration (prototype)
int add(int a, int b);

// Function definition
int add(int a, int b) {
    return a + b;
}
```

**Правило за единична дефиниция (ODR – One Definition Rule):** Всяка функция или обект може да бъде *дефинирана* само веднъж в цялата програма (в рамките на един translation unit). Може да бъде *декларирана* много пъти.

---


През цялата секция ще използваме следната структура от файлове като работен пример. Всички файлове се намират в [`code/math`](code/math):

```
math/
├── math_utils.h      shared declarations (included by any .cpp that needs them)
├── math_utils.cpp    definitions (compiled once into math_utils.o)
├── main.cpp          entry point, uses math_utils and config
│
├── globals.cpp       internal-linkage mutable global via anonymous namespace
├── helpers.cpp       internal-linkage via static (C-style alternative)
│
├── config.h          const globals defined in a header (safe – internal linkage)
└── config.cpp        uses the constants from config.h
```

### Header файлове (.h / .hpp)

Header файловете съдържат **декларации**, които се споделят между `.cpp` файлове. Те се включват с `#include`.

**math_utils.h**
```cpp
#ifndef MATH_UTILS_H   // Include guard - prevents multiple inclusion
#define MATH_UTILS_H
// Alternatively use
#pragma once

/**
 * @note both pragma once and header guards can be used at the same time, though it is not necessary.
*/

int add(int a, int b);
int multiply(int a, int b);

#endif // MATH_UTILS_H
```

### [Още за pragma](https://gcc.gnu.org/onlinedocs/cpp/Pragmas.html)
### [Относно pragma + include guards](https://en.wikipedia.org/wiki/Pragma_once)

**math_utils.cpp**
```cpp
#include "math_utils.h"

int add(int a, int b) { return a + b; }
int multiply(int a, int b) { return a * b; }
```

> Важно - обърнете внимание как .cpp файлове не се включват с `#include`
**main.cpp**
```cpp
#include <iostream>
#include "math_utils.h"  // Include only declarations

#include "math_utils.cpp" // NEVER DO THIS, the compiler allows it, but it will cause multiple definition errors during linking

int main() {
    std::cout << add(3, 4) << std::endl;
}
```

**Ръчна компилация:** \
Компилират се само `.cpp` файловете, а не `.h`! Header файловете се включват в `.cpp` и компилаторът ги обработва автоматично.
```bash
# Do this when compiling by hand
g++ main.cpp math_utils.cpp -o program
# Or, if the system supports wildcards '*':
g++ *.cpp -o program
```

### Области на видимост (Scope)

| Вид              | Описание                                               |
|:-----------------|:-------------------------------------------------------|
| Блоков (local)   | Вътре в `{}` – живее до края на блока                  |
| Файлов (global)  | Извън всички функции – видима за целия файл            |
| Namespace        | Ограничен до даден namespace                           |
| Клас/Структура   | Членове, видими чрез `.`                       |

```cpp
int x = 10; // Global

void foo() {
    int x = 20;     // Local – shadows the global
    {
        int x = 30; // Inner block – shadows the local
        std::cout << x;  // 30
    }
    std::cout << x;      // 20
}
```

### Глобални променливи

Глобалните променливи съществуват за целия живот на програмата и са видими за целия файл. Проблемът е, че по подразбиране те имат **външна свързаност (external linkage)** – което означава, че са видими и в другите translation units и може да предизвикат конфликти. Следва се правилото за единична дефиниция. Ако се опитате да дефинирате една и съща глобална променлива в два различни `.cpp` файла, ще получите грешка при линкване.

За да ограничим видимостта на глобална променлива до само текущия файл, използваме **анонимен namespace**:

**globals.cpp**
```cpp
namespace {
    int counter = 0;          // Can only be seen in globals.cpp

    void incrementCounter() { // Only in globals.cpp
        counter++;
    }
}

void doWork() {
    incrementCounter(); // OK
}
```

**main.cpp**
```cpp
counter++;           // Error: 'counter' was not declared in this scope
incrementCounter();  // Error: 'incrementCounter' was not declared in this scope
```

> **Добра практика:** Избягвайте глобални изменяеми (mutable) променливи. Те правят кода труден за тестване и дебъгване. Ако трябва, ограничете видимостта им с анонимен namespace. Глобалните константи (`const`) също са приемливи.

> `static` при файлов обхват дава вътрешна свързаност – еквивалентно на анонимен namespace, но е наследство от C:

**helpers.cpp**
```cpp
static int helperCount = 0;   // Internal linkage – невидимо извън файла

static void internalHelper() {
    helperCount++;
}
```

### `const` globals и вътрешна свързаност (internal linkage)

В C++ `const` глобалните променливи имат **вътрешна свързаност (internal linkage)** по подразбиране. Това означава, че са видими само в текущия файл и могат безопасно да се дефинират директно в header файлове – всеки `.cpp`, който ги включва, получава свое собствено копие, без ODR нарушение:

**config.h**
```cpp
const int MAX_RETRIES = 5; // Internal linkage – safe to put in a header
```

#### Вмъкване: `const char*` vs `const char* const` – кой указател трябва да е `const`?

Това е важна разлика. `const char*` означава, че **данните** са константни, но **указателят** остава изменяем. Изменяем указател има **external linkage по подразбиране** – дефинирането му в хедър ще причини Multiple Definition грешка при линкване.

**config.h**
```cpp
// BAD: can cause multiple definition errors if included in multiple .cpp files
const char* COLORS[] = { "red", "green", "blue" };

// GOOD
const char* const COLORS[] = { "red", "green", "blue" };

// COLORS[0] = "yellow"; // Error: pointer is const – cannot be reassigned
```

> **Правило:** За глобални масиви от низове почти винаги искате `const char* const[]`. Без `const` на указателя нямате истинска вътрешна свързаност.

---

## Разположение на член-данни в паметта на структури

Компилаторът нарежда член-данните **в реда на деклариране**, но може да добавя **пълнеж (padding)** между тях, за да спази **изискванията за подравняване (alignment)**.

### Правила за подравняване

Всеки тип трябва да започва на адрес, кратен на неговия размер:

| Тип            | Размер | Изисква се адрес кратен на |
|:---------------|:------:|:--------------------------:|
| `char`         | 1 байт | 1                          |
| `short`        | 2 байта| 2                          |
| `bool`         | 1 байт | 1                          |
| `int`          | 4 байта| 4                          |
| `float`        | 4 байта| 4                          |
| `double`       | 8 байта| 8                          |
| `long long`    | 8 байта| 8                          |
| `pointer`      | 8 байта| 8 (на 64-bit система)      |

Размерът на структурата е кратен на подравняването на **най-голямото** поле (tail padding).

### Пример – лошо наредена структура

```cpp
struct Bad {
    char   a;       // 1 byte  [offset 0]
    //               3 bytes padding
    int    b;       // 4 bytes [offset 4]
    char   c;       // 1 byte  [offset 8]
    //               7 bytes padding
    double d;       // 8 bytes [offset 16]
}; // sizeof(Bad) = 24
```

Визуално (всяка клетка = 1 байт):
```
[a][.][.][.][b][b][b][b][c][.][.][.][.][.][.][.][d][d][d][d][d][d][d][d]
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
```

### Пример – добре наредена структура

```cpp
struct Good {
    double d;       // 8 bytes [offset 0]
    int    b;       // 4 bytes [offset 8]
    char   a;       // 1 byte  [offset 12]
    char   c;       // 1 byte  [offset 13]
    //               2 bytes tail padding (to align size to multiple of 8)
}; // sizeof(Good) = 16
```

```
[d][d][d][d][d][d][d][d][b][b][b][b][a][c][.][.]
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
```

> **Най-лесен начин за оптимизиране:** Наредете членовете от **най-голям към най-малък** размер, особено ако имате колекция, сложете я най-отгоре.

### Познай размера

Преди да компилирате, изчислете `sizeof` на долните структури (включете padding):

```cpp
// How many bytes does each struct occupy?

struct A {
    int   x;
    char  y;
    int   z;
};

struct B {
    char  a;
    char  b;
    int   c;
    char  d;
};

struct C {
    double x;
    int    y;
    char   z;
    short  w;
};
```

<details>
<summary>Отговори (разгънете след като сте се опитали)</summary>

```
struct A: sizeof = 12 → [x x x x][y][. . .][z z z z]
struct B: sizeof = 12 → [a][b][. .][c c c c][d][. . .]
struct C: sizeof = 16 → [x x x x x x x x][y y y y][z][.][w w]
```

</details>

---

## Допълнително оптимизиране на член данни в паметта

### Битови полета

Позволяват дефиниране на полета с точен брой **битове**. Удобни са за флагове, хардуерни регистри и компактни структури.

```cpp
struct Flags {
    unsigned int read    : 1;   // 1 bit
    unsigned int write   : 1;   // 1 bit
    unsigned int execute : 1;   // 1 bit
    unsigned int level   : 3;   // 3 bits (values 0-7)
    unsigned int         : 2;   // 2 bits anonymous padding (reserved)
}; // Minimum 1 byte
```

**Употреба:**
```cpp
Flags f = {};
f.read    = 1;
f.write   = 1;
f.execute = 0;
f.level   = 5;

std::cout << f.read;  // 1
```

> **Ограничения:** Не можете да взимате адрес (`&`) на bit field. Подредбата на битовете в байта е implementation-defined (зависи от компилатора и от Endianness).

### Форсирано подравняване

Анонимно bit field с ширина `0` **форсира преход** към следващата граница на базовия тип – останалите битове в текущия unit се пропускат:

```cpp
struct Example {
    unsigned char a : 3;   // 3 bits
    unsigned char b : 2;   // 2 bits (packs into the same byte as 'a')
    unsigned char   : 0;   // Forces padding to the next unsigned char (8-bit boundary)
    unsigned char c : 4;   // 4 bits – starts in a brand new byte
};
```

Това изглежда така:
```
[a][a][a][b][b][.][.][.]
 0  1  2  3  4  5  6  7  <- First unsigned char (Byte 0)
                         <- The last 3 bits are wasted (padded) due to `: 0`

[c][c][c][c][.][.][.][.]
 8  9 10 11 12 13 14 15  <- Second unsigned char (Byte 1)
```

Ако го нямаше:
```cpp
struct Example {
    unsigned char a : 3;
    unsigned char b : 2; 
    unsigned char c : 4; 
};
```

Ще изглежда така:
```
[a][a][a][b][b][c][c][c]
 0  1  2  3  4  5  6  7  <- First unsigned char (Byte 0) is completely full.

[c][.][.][.][.][.][.][.]
 8  9 10 11 12 13 14 15  <- Second unsigned char (Byte 1) holds the leftover bit of 'c'.
```
Базовият тип на `: 0` определя до каква граница се подравнява:

```cpp
struct Sensor {
    unsigned char flags  : 3;   // 3 bits в char (8-bit unit)
    unsigned char        : 0;   // Fills the rest of the current char (5 bits) and forces alignment to the next char
    unsigned char status : 5;
};

struct Register {
    unsigned int lo : 12;
    unsigned int    : 0;
    unsigned int hi : 12;  // New int
};
```

**Разлика между `: N` и `: 0`:**

| Синтаксис          | Ефект                                                          |
|:-------------------|:---------------------------------------------------------------|
| `unsigned int : 2` | Пропуска точно 2 бита                                          |
| `unsigned int : 0` | Пропуска **всички** останали битове до края на текущия unit    |

> **Важно:** Поведението е implementation-defined – стандартът гарантира преминаване към следващата unit граница, но не и точния размер на структурата между компилатори. Практически се ползва при **хардуерни регистри** или **мрежови протоколи**, където полетата трябва да са на точно определени битови позиции. Не може да се ползва като първо поле в структура.

## `public` и `private` в структури

В C++ структурите могат да имат **спецификатори за достъп** – `public` и `private`.

```cpp
struct BankAccount {
private:
    double  balance;     // Cannot be accessed directly from outside
    char*   owner;

public:
    void deposit(double amount) {
        if (amount > 0) balance += amount;
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) balance -= amount;
    }

    double getBalance() { return balance; }
};

int main() {
    BankAccount acc;
    // acc.balance = 1000;  // Error: balance is private
    acc.deposit(1000);      // OK: deposit is public
    std::cout << acc.getBalance();
}
```
Тази практика се нарича **инкапсулация/енкапсулация/капсулация** – скриване на вътрешната реализация и предоставяне на контролиран интерфейс. Ползи:
-   Невъзможно е да се наруши [инвариантът](https://medium.com/code-design/invariants-in-code-design-557c7864a047) на структурата отвън (`balance < 0`).
-   Вътрешната реализация може да се промени, без да се счупват ползвателите.

## Гетъри и сетъри (accessors/mutators)

Гетърите и сетърите са публични методи, които позволяват контролирано четене и писане на член-данни. Те са част от добрата практика за инкапсулация.

```cpp
class Rectangle {
private:
    double width;
    double height;

public:
    Rectangle(double w, double h) : width(w), height(h) {}

    double getWidth() { return width; }
    double getHeight() { return height; }

    void setWidth(double w) { width = w; }
    void setHeight(double h) { height = h; }
};

int main() {
    Rectangle rect(3.0, 4.0);
    std::cout << rect.getWidth() << " x " << rect.getHeight() << std::endl;
    rect.setWidth(5.0);
    std::cout << rect.getWidth() << " x " << rect.getHeight() << std::endl;
}
```

Занапред, добра практика е да пишете структури и класове в разделни `.hpp` и `.cpp` файлове, като дефинициите на методите се поставят в `.cpp`, а декларациите в `.hpp`. Това подобрява четимостта и поддръжката на кода.

Например:

**Point.hpp**
```cpp
#pragma once

class Point {
private:
    double x;
    double y;

public:
    double getX();
    double getY();
};
```

**Point.cpp**
```cpp
#include "Point.hpp"

// Make notice of Point:: before the method names, this is because we are defining the methods outside of the class declaration
double Point::getX() { return x; }
double Point::getY() { return y; }
```

Може да намерите пример и в [`code/point`](code/point).

## Задачи

### Задача: Spaceship Registry (Галактически регистър на кораби)

Вашата задача е да имплементирате система за регистрация на космически кораби.

Всички файлове се намират в [`code/template`](code/template):

```
template/
├── config.h      # Споделени константи (даден, не променяйте)
├── ship.hpp      # Декларации на структурите (попълнете TODO)
├── ship.cpp      # Дефиниции на методите (попълнете TODO)
└── main.cpp      # Тестове (даден, не променяйте)
```

#### Какво трябва да направите

1. **Битови полета (`ShipStatus`):**
   - Дефинирайте битовите полета в `ShipStatus` с правилния брой битове и тип
   - `sizeof(ShipStatus)` трябва да бъде **1 байт**

2. **Enums (`ShipType`):**
   - Във всеки namespace дефинирайте `enum ShipType` с подходящи стойности:
     - `Fleet::ShipType` — `Fighter`, `Cruiser`, `Destroyer`
     - `Civilian::ShipType` — `Transport`, `Shuttle`, `Mining`
   - Имплементирайте `typeToString` функция във всеки namespace, която преобразува enum към текст
   - Обърнете внимание, че двата enum-а имат **едно и също име**, но живеят в различни namespaces

3. **Namespaces (`Fleet` и `Civilian`):**
   - Двата namespace-а съдържат структура с едно и също име `Ship`, но с различни полета
   - `Fleet::Ship` има `crewCount` (брой екипаж)
   - `Civilian::Ship` има `cargoCapacity` (товароносимост)
   - Използвайте пълни квалифицирани имена при дефинициите в `.cpp` файла

4. **public/private и валидация:**
   - Всички член-данни са `private`
   - Достъпът е чрез getter/setter методи
   - Сетърите валидират входните данни:
     - `crewCount` и `cargoCapacity` не могат да бъдат отрицателни
     - `fuelLevel` не може да надвишава `MAX_FUEL` (7)

5. **Разделна компилация:**
   - Декларациите са в `.hpp`, дефинициите в `.cpp`
   - Методите се дефинират с `Namespace::Struct::method()`

#### Преди да компилирате

Изчислете на ръка и после проверете с програмата:
- `sizeof(ShipStatus)` = ?
- `sizeof(Fleet::Ship)` = ?
- `sizeof(Civilian::Ship)` = ?