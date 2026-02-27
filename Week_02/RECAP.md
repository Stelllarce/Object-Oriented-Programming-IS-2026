# Week 02 – Кратко резюме

---

## Съдържание

1. [Header файлове и Include guards](#1-header-файлове-и-include-guards)
2. [Декларация vs Дефиниция](#2-декларация-vs-дефиниция)
3. [Namespaces](#3-namespaces)
4. [Глобални променливи – добри практики](#4-глобални-променливи--добри-практики)
5. [Padding и Alignment](#5-padding-и-alignment)
6. [Битови полета (Bit fields)](#6-битови-полета-bit-fields)
7. [`public` / `private` и инкапсулация](#7-public--private-и-инкапсулация)
8. [Разделяне на класове в .hpp / .cpp](#8-разделяне-на-класове-в-hpp--cpp)
9. [Бърза справка](#бърза-справка)

---

## 1. Header файлове и Include guards

Без include guard един header може да се включи два пъти води до грешка при компилиране.

```cpp
// math_utils.h
#pragma once

int add(int a, int b);      // декларация
int multiply(int a, int b); // декларация
```

> **Header файловете се `#include`-ват. `.cpp` файловете НИКОГА не се включват с `#include`!**

Тооест:
```cpp
#include "math_utils.h" // OK
#include "math_utils.cpp" // НЕ го правете в никакъв случай!
```

**Компилирайте само `.cpp` файлове:**
```bash
g++ main.cpp math_utils.cpp -o program
```

### Свързаност (Linkage)

| Вид | Видим от | Как се постига |
|-----|----------|----------------|
| **Вътрешна (internal)** | Само текущия файл | `static`, анонимен `namespace`, `const` глобална |
| **Външна (external)** | Всички файлове | По подразбиране за функции и глобални променливи |

---

## 2. Декларация vs Дефиниция

| | Декларация | Дефиниция |
|--|-----------|----------|
| Какво прави | Казва *че съществува* | Дава пълно описание, заделя памет |
| Колко пъти | Много пъти | **Само веднъж** (ODR) |

```cpp
int add(int a, int b);          // декларация (в .h)
int add(int a, int b) { ... }   // дефиниция (в .cpp)
```

**ODR (One Definition Rule):** Всяка функция/обект се дефинира **точно веднъж**.

---

## 3. Namespaces

Предотвратяват конфликти на имена.

```cpp
namespace Geometry {
    struct Point { double x, y; };
}
namespace Physics {
    struct Point { double x, y, z; }; // различен Point, без конфликт
}

Geometry::Point p = {1.0, 2.0};
```

**Анонимен namespace** води до вътрешна свързаност (само за текущия файл):
```cpp
namespace {
    int secret = 42; // невидим извън файла
}
```

---

## 4. Глобални променливи – добри практики

```cpp
// BAD: видима в целия проект, риск от конфликти
int counter = 0;

// GOOD: ограничена до текущия файл
namespace { int counter = 0; }  // анонимен namespace
```

`const` глобалните имат вътрешна свързаност по подразбиране води до безопасно е да се слагат в header:
```cpp
const int MAX_RETRIES = 5; // може директно в .h
```

---

## 5. Padding и Alignment

Компилаторът добавя **padding байтове**, за да подравни полетата.  
Всеки тип трябва да започва на адрес, кратен на размера му.

```cpp
struct Bad {             // sizeof = 24
    char   a;   // [0]  води до 3 байта padding
    int    b;   // [4]
    char   c;   // [8]  води до 7 байта padding
    double d;   // [16]
};

struct Good {            // sizeof = 16
    double d;   // [0]
    int    b;   // [8]
    char   a;   // [12]
    char   c;   // [13]  води до 2 байта tail padding
};
```

> **Правило:** Наредете полетата от **най-голям към най-малък** размер.

---

## 6. Битови полета (Bit fields)

Позволяват поле с точен брой **битове**.

```cpp
struct Flags {
    unsigned int read    : 1;  // 1 бит
    unsigned int write   : 1;  // 1 бит
    unsigned int execute : 1;  // 1 бит
    unsigned int level   : 3;  // стойности 0-7
    unsigned int         : 2;  // анонимен padding (2 бита)
};

Flags f = {};
f.read  = 1;
f.level = 5;
```

- Не можете да взимате `&` на bit field.
- `unsigned int : 0` води до принудително преминаване към следващата граница.

---

## 7. `public` / `private` и инкапсулация

```cpp
struct BankAccount {
private:
    double balance;

public:
    void deposit(double amount) {
        if (amount > 0) balance += amount;
    }
    double getBalance() { return balance; }
};

// acc.balance = 1000; // ГРЕШКА: private
acc.deposit(1000);     // OK
```

**Инкапсулация** = скриване на вътрешните данни, достъп само чрез публични методи.  
Гарантира валидно вътрешно състояние (инвариант).

---

## 8. Разделяне на класове в .hpp / .cpp

**Point.hpp**
```cpp
#pragma once

class Point {
private:
    double x, y;
public:
    double getX();
    double getY();
    void   setX(double v);
    void   setY(double v);
};
```

**Point.cpp**
```cpp
#include "Point.hpp"

double Point::getX() { return x; }
double Point::getY() { return y; }
void   Point::setX(double v) { x = v; }
void   Point::setY(double v) { y = v; }
```

---

## Бърза справка

| Концепция | Ключова дума/синтаксис |
|-----------|----------------------|
| Вътрешна свързаност | `static` / анонимен `namespace {}` |
| Include guard | `#ifndef` / `#pragma once` |
| Дефиниция на метод извън клас | `ClassName::methodName()` |
| Размер в байтове | `sizeof(Type)` |
| Bit field | `unsigned int field : N;` |
| Само четене на член | getter: `double getX() { return x; }` |
