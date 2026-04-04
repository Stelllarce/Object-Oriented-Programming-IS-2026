# Класове. ООП принципи. SOLID. RAII. Конструктори и деструктори. Семантики за притежаване.

---

## Съдържание

1. [Принципи на ООП](#принципи-на-ооп)
2. [SOLID принципи](#solid-принципи)
3. [Класове – декларация и дефиниция](#класове--декларация-и-дефиниция)
4. [Кръгови зависимости и forward declaration](#кръгови-зависимости-и-forward-declaration)
5. [Конструктори и деструктори](#конструктори-и-деструктори)
6. [Указател `this`](#указател-this)
7. [RAII (Resource Acquisition Is Initialization)](#raii-resource-acquisition-is-initialization)
8. [Член-данни – ограничения и семантики за притежаване](#член-данни--ограничения-и-семантики-за-притежаване)
9. [Const методи](#const-методи)

---

## Принципи на ООП

Обектно-ориентираното програмиране се основава на четири принципа:

| Принцип | Описание |
|:--------|:---------|
| **Абстракция** | Скриване на несъществените детайли и показване само на важното. Потребителят работи с опростен интерфейс. |
| **Капсулация** | Обединяване на данни и методи в едно цяло (клас) и контролиран достъп до тях чрез `private`/`public`. |
| **Наследяване** | Създаване на нов клас на базата на съществуващ – преизползване на код и разширяване на функционалност. |
| **Полиморфизъм** | Едно и също действие може да има различно поведение за различни типове обекти. |

В рамките на тази седмица ще се фокусираме главно върху **абстракция** и **капсулация**. Наследяването и полиморфизмът ще бъдат разгледани в бъдещи седмици.

```cpp
// Encapsulation example – the user doesn't need to know how fuel is stored internally
class Spaceship {
private:
    int fuel;

public:
    void refuel(int amount) {
        if (amount > 0) fuel += amount;
    }
    int getFuel() const { return fuel; }
};

// Here the Spaceship is an abstraction - it exposes a simple interface for refueling and checking fuel, hiding the internal details of how fuel is managed.
```

---

## SOLID принципи

SOLID е набор от пет принципа за проектиране на поддържаем обектно-ориентиран код. Те са по-скоро рамка за мислене, знайте ги и ги следвайте когато е уместно, но не превръщайте кода си в по-сложен само заради тях.

| Принцип | Описание |
|:--------|:---------|
| **S** – Single Responsibility | Клас трябва да има **една причина за промяна** – отговорност за единствено нещо. |
| **O** – Open/Closed | Класовете трябва да са **отворени за разширяване**, но **затворени за модификация**. |
| **L** – Liskov Substitution | Обект от подклас трябва да може да замени обект от базовия клас **без да счупи** програмата. |
| **I** – Interface Segregation | По-добре **много малки** интерфейси, отколкото един голям. |
| **D** – Dependency Inversion | Зависете от **абстракции**, не от конкретни реализации. |

> **Забележка:** На този етап е достатъчно да разбирате **S** и **O**. Останалите ще придобият повече смисъл когато започнем да работим с наследяване и полиморфизъм.

### Single Responsibility – пример

За да разделим отговорността, можем да създадем два оотделни класа, които да се грижат за отделните функционалностти, които трябва да поддържаме. Ако тези функционалности зависят от първоначалния клас, можем да се възползваме от `dependency injection` - вкарваме зависимия клас като параметър в метода. Това се нарича още `method injection`:

```cpp
// BAD: one class does too many things
class Report {
    void generateReport();
    void saveToFile();       // file I/O responsibility
    void sendByEmail();      // network responsibility
};

// GOOD: each class has one job
class Report {
    void generateReport();
};

class ReportSaver {
    void saveToFile(const Report& r);
};

class Email {
    void sendByEmail(const Report& r);
};
```

### Open/Closed – пример

```cpp
// BAD: adding a new shape requires modifying existing code
double area(const char* shape, double a, double b) {
    if (strcmp(shape, "rect") == 0) return a * b;
    if (strcmp(shape, "circle") == 0) return 3.14 * a * a;
    // ... every new shape means editing this function
    return 0;
}

// GOOD: new shapes can be added without modifying existing code
using AreaFunc = double(*)(double, double);
double RectArea(double width, double height) { return width * height; }
double CircleArea(double radius, double) { return 3.14 * radius * radius; }

double area(AreaFunc func, double a, double b) {
    return func(a, b);  // Open for extension – we can pass any area function
}
```

---

## Класове – декларация и дефиниция

### `struct` срещу `class`

Единствената разлика: в `struct` членовете са **`public`** по подразбиране, а в `class` – **`private`**. Конвенция: `struct` за прости типове без инваранти, `class` за типове с логика и скрито състояние.

```cpp
struct Point { // Everything public by default
    double x;
    double y;
};

class Circle { // Everything private by default
    double radius;
public:
    double getRadius() const { return radius; }
};
```

### Декларация и дефиниция на клас

Както при структури – декларациите в `.hpp`, дефинициите в `.cpp`:

**circle.hpp**
```cpp
#pragma once

class Circle {
private:
    double radius;
    double x, y;

public:
    Circle(double r, double cx, double cy);

    double getRadius() const;
    double area() const;
};
```

**circle.cpp**
```cpp
#include "circle.hpp"

// Note the ClassName:: prefix when defining methods outside the class
Circle::Circle(double r, double cx, double cy)
    : radius(r), x(cx), y(cy) {}

double Circle::getRadius() const { return radius; }

double Circle::area() const { return 3.14159 * radius * radius; }
```

```bash
g++ main.cpp circle.cpp -o program
```

### Достъпване на член данни през указател към клас

Когато имаме указател към обект от клас, използваме `->` за достъп до член-данни и методи:

```cpp
Circle* c = new Circle(5.0, 0.0, 0.0);
std::cout << "Radius: " << c->getRadius() << std::endl; // Use -> to call method on pointer
// this is equivallent to
// std::cout << "Radius: " << (*c).getRadius() << std::endl; // Dereference pointer, then call method
```

Синтактичната захар е създадена за удобство. Ще се използва много често оттук нататък.

### Член-данни не могат да бъдат инстанции от собствения клас

Когато компилаторът среща декларация на клас, той трябва да знае **размера** на всички член-данни. Ако клас съдържа себе си, размерът е безкраен:

```cpp
class Node {
    int value;
    Node next;  // Error: incomplete type – Node's size is not yet known
};
```

Решението е да се използва **указател** (или референция), защото неговият размер е фиксиран (8 байта на 64-bit система):

```cpp
class Node {
    int value;
    Node* next;  // OK: pointer has a known, fixed size
    Node& nextRef; // Also OK: reference has a known, fixed size
};
```

### Задача 1: Базова структура на низ

Създайте пространство от имена `my_std` и в него дефинирайте шаблонен клас `basic_string<T>`. Добавете член-данни за указател към динамично заделен масив от тип `T`, размер (брой елементи) и капацитет (заделена памет).`
Освен това създайте `using string = my_std::basic_string<char>;` извън класа (но в пространството от имена), за да можете лесно да използвате `my_std::string`.

---

## Кръгови зависимости и forward declaration

### Проблемът

Ако два класа се включват взаимно чрез `#include`, получаваме кръгова зависимост:

**engine.hpp** (проблемен)
```cpp
#pragma once
#include "car.hpp"   // includes car.hpp, which includes engine.hpp...

class Engine {
    Car* owner;      // Needs to know about Car
};
```

**car.hpp** (проблемен)
```cpp
#pragma once
#include "engine.hpp" // includes engine.hpp, which includes car.hpp...

class Car {
    Engine engine;     // Needs to know about Engine
};
```

Резултат: безкрайна рекурсия на `#include` → грешка при компилация.

### Решение: forward declaration

Когато се нуждаем само от **указател или референция** към даден клас, не е нужен `#include` на целия header – достатъчна е **forward декларация** (`class ClassName;`). Тя казва на компилатора, че типът съществува, без да разкрива структурата му:

**engine.hpp** (коректен)
```cpp
#pragma once

class Car;  // Forward declaration – no #include needed

class Engine {
    Car* owner;  // OK: only a pointer, no need for Car's full definition
public:
    void setOwner(Car* c);
};
```

**car.hpp** (коректен)
```cpp
#pragma once
#include "engine.hpp"  // Full include – Car contains Engine by value

class Car {
    Engine engine;
public:
    Engine& getEngine();
};
```

В `.cpp` файловете вече включваме пълните header-и:

**engine.cpp**
```cpp
#include "engine.hpp"
#include "car.hpp"  // Now we need the full definition to use Car's methods

void Engine::setOwner(Car* c) { owner = c; }
```

> **Правило:** Използвайте forward declaration когато header файлът се нуждае само от указател/референция към друг клас. Включвайте пълен `#include` само когато трябва да знаете размера или да викате методи на класа.

### Кога forward declaration НЕ е достатъчна

| Нужда | Forward declaration | Пълен `#include` |
|:------|:-------------------:|:----------------:|
| Указател (`T*`) | ✅ | |
| Референция (`T&`) | ✅ | |
| Член-данна по стойност (`T obj`) | | ✅ |
| Извикване на метод на `T` | | ✅ |
| Наследяване от `T` | | ✅ |
| `sizeof(T)` | | ✅ |

Пълен работен пример може да се намери в [`code/circular`](code/circular).

---

## Конструктори и деструктори

### Конструктори

Конструкторът е специален метод, който се извиква автоматично при създаване на обект. Няма върнат тип и носи името на класа:

```cpp
class Timer {
    int seconds;
public:
    Timer() : seconds(0)  {}  // Default constructor
    Timer(int s) : seconds(s)  {}  // Parameterized constructor
};

Timer t1;       // Calls Timer() → seconds = 0
Timer t2(30);   // Calls Timer(30) → seconds = 30
Timer t3{45};   // Same as above, using brace initialization
```

### Member initializer list

Инициализирането на член-данни се прави чрез **member initializer list** (след `:`, преди `{}`). Това е **по-ефективно** от присвояване в тялото, защото данните се инициализират директно, без първо да се конструират по подразбиране и после да се презаписват. За референции и `const` полета е **задължително**:

```cpp
class Pair {
    const int id;
    int value;
public:
    // GOOD: uses member initializer list
    Pair(int i, int v) : id(i), value(v) {}

    // BAD: won't compile – id is const and cannot be assigned
    // Pair(int i, int v) {
    //     id = i;    // Error: cannot assign to const
    //     value = v; // Works, but is less efficient
    // }
};
```

> **Важно:** Членовете се инициализират в **реда на деклариране** в класа, а не в реда в initializer list-а.

### Конструктор по подразбиране (Default constructor)

Ако не дефинирате **никакъв** конструктор, компилаторът генерира default constructor, който инициализира член-данните по подразбиране (за примитивни типове – стойностите са неопределени). Ако дефинирате **поне един** конструктор, default constructor **не** се генерира автоматично:

```cpp
class A {
    int x;
public:
    A(int val) : x(val) {}
};

A a1(5);   // OK
// A a2;   // Error: no default constructor
```

Ако искате да си го върнете:

```cpp
class A {
    int x;
public:
    A() = default;          // Explicitly request default constructor
    A(int val) : x(val) {}
};

A a1;    // OK: uses default constructor
A a2(5); // OK: uses parameterized constructor
```

> **Важно**: при създаване на статични масиви от класове е необходимо да има `default constructor`, защото за разлика от указателите, в тези масиви всеки елемент трябва да бъде контруиран, за което се извиква кострукторът по подразбиране.

### Деструктор

Деструкторът се извиква автоматично когато обектът излезе от обхват (scope) и служи за освобождаване на ресурси. Най-често се използва за освобождаване на динамична памет, но в него могат и да се изпълняват всякакви действия, които трябва да се случат при унищожаване на обекта:

```cpp
class Buffer {
    char* data;
    size_t size;
public:
    Buffer(size_t n) : data(new char[n]), size(n) {}

    ~Buffer() {
        delete[] data;  // Free allocated memory
    }
};

void example() {
    Buffer buf(1024);
    // ... use buf ...
}  // buf goes out of scope → ~Buffer() is called automatically → memory is freed
```

> **Правило:** Ако клас заделя ресурси (динамична памет, файлови дескриптори), винаги дефинирайте деструктор, който ги освобождава.
> НИКОГА не извиквайте деструктора експлицитно (т.е. `obj.~ClassName()`) – това е отговорност на компилатора и ръчното му извикване може да доведе до двойно освобождаване (double free) и други проблеми.

Пълен пример: [`code/constructors`](code/constructors).

### Извикване на конструктор и деструктор при агрегатни типове
```cpp
class Engine {
public:
    Engine()  { std::cout << "Engine constructed\n"; }
    ~Engine() { std::cout << "Engine destroyed\n"; }
};

class Wheels {
public:
    Wheels()  { std::cout << "Wheels constructed\n"; }
    ~Wheels() { std::cout << "Wheels destroyed\n"; }
};

class Car {
    Engine engine;   // declared first → constructed first
    Wheels wheels;
public:
    Car() : wheels(), engine() /* Even though wheels is invoked first here, the order of 
    initialization is determined by the declaration order in the class */ 
    { std::cout << "Car constructed\n"; }
    ~Car() { std::cout << "Car destroyed\n"; }
};
```

Създаване на обект от тип `Car` ще доведе до следния изход:
```
Engine constructed
Wheels constructed
Car constructed
```

Освобождаването ще се случи в обратен ред:
```
Car destroyed
Wheels destroyed
Engine destroyed
```

## Указател `this`

Всеки нестатичен метод има достъп до специален указател `this`, който сочи към текущия обект. Това е полезно за достъп до член-данни и методи, особено когато има именни конфликти:

```cpp
class Point {
    double x, y;
public:
    Point(double x, double y) : x(x), y(y) {}

    void setX(double x) {
        this->x = x; // Use this-> to disambiguate member x from parameter x
        // x = x; Allowed, modern compilers know the difference
    }

    void setY(double y) {
        this->y = y; // Use this-> to disambiguate member y from parameter y
    }

    // With this we can also call other methods of the same object:
    void changeCoords(double dx, double dy) {
        this->setX(this->x + dx); // Call setX on the current object
        this->setY(this->y + dy); // Call setY on the current object

        // Or simply:
        // setX(x + dx);
        // setY(y + dy);
    }

    // We can also use the this pointer to return the current object in some form:
    Point* getThis() {
        return this; // Returns a pointer to the current object
    }
    
    // Or perhaps give it to another method:
    void move(Point& p) {
        return moveTo(*this, p); // Pass the current object to another method
    }
};

void moveTo(Point& from, Point& to) {
    // Move logic here
}
```

> Вметка: ако не искаме да се объркваме с конфликт на имена, има конвенции на места да пишат член данните с префиксен или постфиксен "_" или "m_" (member), за да се различават от параметрите. Например: `this->m_x = x;` или `this->x_ = x;`.

---

## RAII (Resource Acquisition Is Initialization)

RAII е идиом в C++, при който **придобиването на ресурс** е обвързано с инициализацията на обект, а **освобождаването** – с неговото унищожаване:

| Момент | Действие |
|:-------|:---------|
| Конструктор | Придобива ресурс (заделя памет, отваря файл) |
| Деструктор | Освобождава ресурс (освобождава памет, затваря файл) |

Предимства:
-   Няма memory leaks – ресурсът се освобождава автоматично при излизане от обхват.
-   Exception-safe – дори при изключение деструкторът се извиква.
-   Код без ръчни `delete`/`fclose` разпръснати из програмата.

```cpp
#include <fstream>
#include <iostream>

class FileWriter {
    std::ofstream file;
public:
    FileWriter(const char* filename) : file(filename) {
        if (!file.is_open()) {
            std::cerr << "Failed to open " << filename << std::endl;
        }
    }

    void write(const char* text) {
        file << text;
    }

    // No explicit destructor needed – std::ofstream closes itself (RAII within RAII)
};

void writeLog() {
    FileWriter log("log.txt");
    log.write("Started\n");
    log.write("Finished\n");
}  // log goes out of scope → file is closed automatically
```

Друг пример – клас, който управлява динамичен масив:

```cpp
class IntArray {
    int* data;
    size_t size;
public:
    IntArray(size_t n) : data(new int[n]()), size(n) {}  // Acquire

    ~IntArray() { delete[] data; } // Release

    int& operator[](size_t i) { return data[i]; }
    size_t getSize() const { return size; }
};

void example() {
    IntArray arr(100);
    arr[0] = 42;
    // No need to call delete – the destructor handles it
}
```

Допълнително четене: най-явния пример за RAII са [умните указатели](https://learn.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-170), които вие **няма** да можете да използвате в рамките на курса, но можете да погледнете ако ви е интересно.

### Задача 2: Конструктори, деструктор и RAII

Добавете към `my_std::basic_string<T>`:
- Конструктор по подразбиране, който създава празен низ (и заделя минимален капацитет).
- `void resize(size_t newSize)` - променя размера на низа, като запазва съществуващите данни (ако е възможно) и добавя терминираща нула `\0` при нужда. Ако новият размер е по-голям от капацитета, заделяйте нова памет. Направете я `private`.
- Конструктор с параметър от тип `const char*`, който инициализира низа с подадения масив, като задели нужната памет.
- Конструктор, който добавя n на брой копия на символа c (например `my_std::string s(5, 'a');` ще създаде низ "aaaaa").
- Метод `void clear()`, който изчиства низа (прави го празен, но не освобождава паметта).
- Деструктор, който освобождава заделената памет.

---

## Член-данни – ограничения и семантики за притежаване

### Член-данна не може да бъде инстанция от собствения клас

Както казахме по-горе, клас не може да съдържа себе си по стойност, но може да съдържа **указател** или **референция** към себе си:

```cpp
class TreeNode {
    int value;
    TreeNode* left;    // OK: pointer
    TreeNode* right;   // OK: pointer
};
```

### Семантики за притежаване (Ownership semantics)

Когато клас има член-данна, важен въпрос е: **кой притежава този ресурс?**

| Тип член-данна | Притежание | Живот |
|:---------------|:-----------|:------|
| `T obj` (по стойност) | Класът **притежава** обекта | Създава се и се унищожава с класа |
| `T* ptr` (указател) | Зависи от контекста – класът *може* да притежава | Трябва ръчно управление (`new`/`delete`) |
| `T& ref` (референция) | Класът **НЕ притежава** обекта | Трябва обектът да живее по-дълго от класа |

#### По стойност – пълно притежание

```cpp
class Car {
    Engine engine;  // Car owns the Engine – created and destroyed with Car
public:
    Car() : engine(200) {}
    // Engine is automatically destroyed when Car is destroyed
};
```

#### Указател – възможно притежание

```cpp
class Team {
    Player* captain;  // Does Team own the Player? Depends on intent.
public:
    // If we allocate – we must deallocate (owning pointer)
    Team() : captain(new Player("Ivan")) {}
    ~Team() { delete captain; }

    // Or we just reference someone else's Player (non-owning pointer)
    // Team(Player* p) : captain(p) {}
    // In this case – do NOT delete in destructor
};
```

#### Референция – без притежание

Референция като член-данна означава, че обектът **наблюдава** друг обект, без да го притежава. Референцията **трябва** да се инициализира в member initializer list и **не може** да се пренасочва:

```cpp
class Logger {
    std::ostream& output;  // Does NOT own the stream
public:
    Logger(std::ostream& os) : output(os) {}

    void log(const char* msg) {
        output << "[LOG] " << msg << "\n";
    }
};

void example() {
    Logger logger(std::cout);  // logger uses std::cout, doesn't own it
    logger.log("Hello");
}
```

> **Внимание:** Обектът, към който сочи референцията, трябва да **надживее** обекта, който я държи. Ако обектът бъде унищожен преди – получаваме dangling reference (undefined behavior).

> **За този конкретен пример с `std::ostream`**: Потоците не могат да бъдат обикновени член данни по стойност. Затова можем да ги слагаме само като указатели или референции. Защо ще разберем другия път, когато научим какво е копиращ конструктор.

Пълен пример: [`code/ownership`](code/ownership).

---

## Const методи

Метод, обозначен с `const`, обещава, че **няма да променя** състоянието на обекта (член-данните). Най-често *гетърите* са `const`, защото те само връщат информация, без да я променят:

```cpp
class Counter {
    int count;
public:
    Counter(int c) : count(c) {}

    int getCount() const { return count; }      // OK: only reads

    // void increment() const { count++; }      // Error: modifies member in const method

    void increment() { count++; }               // OK: non-const method
};
```

### Защо са важни?

При подаване на обект по `const` референция, можем да викаме **само** `const` методи:

```cpp
void print(const Counter& c) {
    std::cout << c.getCount();  // OK: getCount() is const
    // c.increment();           // Error: increment() is not const
}
```

Също така, `const` методите задължават в тях да се извикват само други `const` методи, което гарантира, че няма неволно модифициране на обекта.

```cpp
class Counter {
    int count;
public:
    Counter(int c) : count(c) {}

    int getCount() const { return count; }      // OK: only reads
    void increment() { count++; }               // OK: non-const method

    void printAndIncrement() const {
        std::cout << getCount();  // OK: getCount() is const
        // increment();           // Error: cannot call non-const method from const method
    }
};
```

> **Правило:** Всички методи, които не модифицират обекта, трябва да бъдат `const`. Това подобрява безопасността и позволява работа с `const` обекти и референции.

### Домашна задача: Довършване на базовата функционалност

Добавете следните методи към вашия `my_std::basic_string<T>`:
- `const T* c_str() const` - връща указател към вътрешния масив (добавяйки терминираща нула `\0`, ако е необходимо). Помислете защо методът е `const` и защо връща `const T*`.
- `T& at(size_t index)` и `const T& at(size_t index) const` - връщат елемент на даден индекс. Хвърляйте изключение (например `std::out_of_range`), ако индексът е невалиден.
- `size_t size() const` - връща текущия размер на низа.
- `size_t capacity() const` - връща наличния капацитет.
- `void push_back(T c)` - добавя символ в края на низа, увеличавайки размера с 1. Ако няма достатъчно капацитет, заделяйте нова памет.

*През следващите седмици ще надграждаме този клас, за да го превърнем в пълна библиотека за работа с низове.*

---

## Допълнителни упражнения

1. **Верижни операции и управление на паметта**
   Създайте клас `DynamicArray`, който съхранява динамично заделен масив от цели числа. 
   - Имплементирайте конструктор, който заделя паметта, и деструктор, който я освобождава.
   - Добавете методи `add(int val)` (добавя стойност към всички елементи) и `multiply(int val)` (умножава всички елементи). Тези методи трябва да връщат референция към текущия обект (`*this`), за да позволят верижно извикване (chaining): `arr.add(5).multiply(2);`
   - Добавете метод `void print() const`, който отпечатва масива.

2. **Двупосочна връзка и притежание**
   Създайте система със служители и мениджъри.
   - Класът `Manager` притежава списък от служители (напр. масив от `Employee*`). `Manager` отговаря за техния жизнен цикъл - заделя ги динамично и задължително ги унищожава в своя деструктор.
   - Класът `Employee` пази указател назад към своя `Manager`, за да може да достъпва информация за екипа си. Тъй като не "притежава" мениджъра, `Employee` не трябва да се опитва да го изтрива.
   - Двата класа се реферират един друг. Използвайте *forward declaration*, за да избегнете грешки с кръгови зависимости при компилация.
   - Направете гетърите в класовете (напр. `getName()`, `getTeamSize()`).

3. **Гъвкава система за логване**
   Напишете два отделни компонента: `MessageFormatter` и `Logger`.
   - `MessageFormatter` е отговорен само за форматиране на съобщение (например, добавя текущата дата/час или префикс `[ERROR]`).
   - `Logger` приема форматираното съобщение и го отпечатва на конзолата. За да не закодирате специфичен форматер, подайте обект от тип `MessageFormatter` като константна референция или указател в конструктора на `Logger`.
   - По този начин `Logger` е отворен за разширяване - можем лесно да напишем и подадем нов форматер (например `HtmlFormatter`), без да променяме кода на самия `Logger`.
   - Уверете се, че `Logger` само "наблюдава" форматера и не се опитва да управлява паметта му.


