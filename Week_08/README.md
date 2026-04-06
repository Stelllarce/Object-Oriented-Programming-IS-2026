# Семантики на преместване, статични член данни и функции, приятелски класове и приятелски функции, контрол над създаването на обекти, шаблони за дизайн (Singleton, Factory, Builder)

## Съдържание

1. [lvalue и rvalue](#lvalue-и-rvalue)
2. [lvalue референции и rvalue референции](#lvalue-референции-и-rvalue-референции)
3. [Семантика на преместване](#семантика-на-преместване)
4. [Голяма шестица (Rule of Five)](#голяма-шестица-rule-of-five)
5. [Статични член данни и функции](#статични-член-данни-и-функции)
6. [Приятелски класове и приятелски функции](#приятелски-класове-и-приятелски-функции)
7. [Контрол над създаването на обекти](#контрол-над-създаването-на-обекти)
8. [Шаблони за дизайн](#шаблони-за-дизайн)
   - [Singleton](#singleton)
   - [Builder](#builder)

---

## lvalue и rvalue

Всеки израз в C++ има **категория** – тя определя дали изразът именува обект с продължителност в паметта или е временна стойност без постоянен адрес.

### lvalue

**lvalue** (left value) е израз, който именува конкретно място в паметта. Можете да вземете адреса му с `&`. Думата "left" идва от това, че може да стои **вляво** от `=`.

```cpp
int x = 10;
int arr[3] = {1, 2, 3};

// All of the following are lvalues – they have a named memory location
x = 20;          // x is an lvalue
arr[0] = 5;      // array element is an lvalue
int& ref = x;    // ref is an lvalue reference to x

int* ptr = &x;   // OK – we can take the address of an lvalue
```

### rvalue

**rvalue** (right value) е временна стойност, която **не притежава постоянен адрес** в паметта. Тя съществува само за времето на израза. Не може да стои вляво от `=`.

```cpp
int x = 10;

// All of the following are rvalues – temporary, no persistent address
int y = x + 5;   // (x + 5) is an rvalue – a temporary result
int z = 42;      // 42 is an rvalue literal

// int* ptr = &(x + 5);  // ERROR – cannot take the address of an rvalue
// (x + 5) = 99;         // ERROR – cannot assign to an rvalue
```

### Бърз тест: lvalue или rvalue?

Прост начин да разберете: **може ли да вземете адреса с `&`?**
- Да → lvalue
- Не → rvalue

```cpp
int a = 5;
&a;           // OK   → a is an lvalue
// &5;        // ERROR → 5 is an rvalue
// &(a + 1);  // ERROR → (a + 1) is an rvalue
&++a;         // OK   → pre-increment returns lvalue
// &a++;      // ERROR → post-increment returns rvalue (temporary copy)
```

> **Забележка:** В C++11 категориите са разширени (lvalue, prvalue, xvalue, glvalue, rvalue), но за практически цели разграничението lvalue/rvalue е достатъчно на този етап.

---

## lvalue референции и rvalue референции

### lvalue референция (`T&`)

Класическата референция – свързва се с **lvalue** (именуван обект). Не може да се свърже директно с временна стойност.

```cpp
int x = 10;

int& ref = x;     // OK – ref binds to lvalue x
ref = 20;         // changes x to 20

// int& bad = 5;  // ERROR – cannot bind lvalue reference to rvalue literal

// Exception: const lvalue reference CAN bind to rvalue (extends its lifetime)
const int& cref = 5;        // OK – lifetime of temporary is extended
const int& cref2 = x + 1;  // OK – temporary result kept alive
```

`const T&` е специален случай – може да се свърже с rvalue, защото обещава, че няма да го модифицира. Затова функциите, приемащи `const T&`, работят и с временни стойности.

```cpp
void print(const int& val) {
    // accepts both lvalues and rvalues
}

int n = 42;
print(n);       // OK – lvalue
print(n + 1);   // OK – rvalue (temporary result)
print(7);       // OK – rvalue literal
```

### rvalue референция (`T&&`)

Въведена в C++11. Свързва се **само с rvalue** – временни обекти без постоянен адрес. Позволява ни да **открадваме ресурсите** на временни обекти вместо да ги копираме.

```cpp
int x = 10;

// int&& bad = x;     // ERROR – x is an lvalue, not an rvalue
int&& rref = 5;       // OK – binds to rvalue literal
int&& rref2 = x + 1; // OK – binds to temporary result

rref = 99;   // we can modify through the rvalue reference
```

### Претоварване по категория на аргумента

Можем да пишем **две претоварени версии** на функция – едната приема lvalue, другата приема rvalue. Компилаторът избира правилната автоматично:

```cpp
void process(const int& val) {
    // Called for lvalues – observe only
}

void process(int&& val) {
    // Called for rvalues – safe to consume or modify
}

int n = 5;
process(n);            // calls lvalue overload
process(10);           // calls rvalue overload (literal)
process(std::move(n)); // calls rvalue overload (explicit move)
// n is now in a valid-but-unspecified state
```

### `std::move` – принудително преобразуване към rvalue

`std::move` **не премества нищо**. Той само казва на компилатора: *можеш да пренесеш данните от този обект в другия ефикасно*. Резултатната стойност от `std::move` е **xvalue**, но в случая е неразличима от rvalue. [Относно стойностните категории](https://en.cppreference.com/w/cpp/language/value_category.html).

```cpp
class Object {
    Object() = default;
};
void sink(Object&& s);  // binds to both xvalues and prvalues

Object s = Object();
sink(std::move(s)); // xvalue — binds
sink(Object());     // prvalue — binds
sink(s);            // lvalue — does NOT bind (compiler error)
```

> След `std::move(a)` единствените безопасни операции върху `a` са: **преприсвояване** или **унищожаване**.

### Обобщение

| | `T&` | `const T&` | `T&&` |
|:---|:---:|:---:|:---:|
| Свързва се с lvalue | ✅ | ✅ | ❌ |
| Свързва се с rvalue | ❌ | ✅ | ✅ |
| Свързва се с xvalue | ❌ | ✅ | ✅ |
| Позволява модификация | ✅ | ❌ | ✅ |
| Типична употреба | in/out параметър | read-only параметър | move семантика |

### Задача

Към класа `my_std::basic_string<T>` добавете следните два метода:

- void assign(const basic_string&& str) noexcept – премества съдържанието на `str` в текущия обект, оставяйки `str` в валидно, но неопределено състояние.
- void assign(const basic_string& str) – копира съдържанието на `str` в текущия обект.

---

## Семантика на преместване

Преместването (move) позволява **прехвърляне на ресурс** от един обект в друг без копиране. Изходният обект се оставя в **валидно, но неопределено** (valid-but-unspecified) състояние.

```cpp
class DynamicArray {
    int* data;
    int  size;

public:
    DynamicArray(int n) : size(n), data(new int[n]) {}

    // Move constructor – steal the resource, leave the source empty
    DynamicArray(DynamicArray&& other) noexcept
        : size(other.size), data(other.data)
    {
        other.data = nullptr;
        other.size = 0;
    }

    // Move assignment operator
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this == &other) return *this;

        delete[] data;      // free current resource

        data = other.data;  // steal
        size = other.size;

        other.data = nullptr;
        other.size = 0;

        return *this;
    }

    ~DynamicArray() { delete[] data; }
};
```

```cpp
DynamicArray a(5);
DynamicArray b = std::move(a);  // move constructor – a is now empty
b = DynamicArray(10);           // move assignment – temporary is moved into b
```

> **Забележка:** `noexcept` при move операции е важно – стандартната библиотека го използва, за да реши дкой конструктор ще извика - за копиране или преместване (напр. при `std::vector` resize).

### Упражнение: Конкатенация и move оптимизация

Добавете операции за конкатенация към `my_std::basic_string<T>`:

- `basic_string& operator+=(const basic_string& rhs)`
- `basic_string& operator+=(T ch)`
- Нечлен-функция `operator+(const basic_string& lhs, const basic_string& rhs)`

Условия:

- При недостиг на капацитет използвайте стратегия за растеж (например удвояване).
- При невъзможна алокация/прекомерен размер хвърляйте `std::length_error`.
- Реализирайте `operator+` така, че да се възползва от move семантика при временни обекти.

---

## Голяма шестица (Rule of Five)

Същата като Rule of Three, но добавя **конструктор за преместване** и **оператор за преместващо присвояване**. Така необходимите методи стават:

- **Конструктор по подразбиране**
- **Деструктор**
- **Конструктор за копиране**
- **Оператор за копиращо присвояване**
- **Конструктор за преместване**
- **Оператор за преместващо присвояване**

### Кое правило да изберем?

| Ситуация | Правило |
|:---------|:--------|
| Само стойностни типове (без динамична памет) и RAII обвивки като член-данни | **Rule of Zero** |
| Управляваме ресурс и възнамеряваме да копираме обекта | **Rule of Three** |
| Обектът е скъп за копиране | **Rule of Five** |

### Задача:

Добавете следните методи към класа `my_std::basic_string<T>`:

- Конструктор за преместване – „открадва" ресурса, оставя източника с `nullptr` и нулев размер.
- Оператор за преместващо присвояване – освобождава текущия ресурс, откражда от източника.

---

## Статични член данни и функции

### Статични член данни

**Статичните член данни** принадлежат на **класа**, а не на конкретна инстанция. Всички обекти от даден клас споделят едно и също статично поле. Статичният член се **декларира** вътре в класа, но се **дефинира** извън него.

**Counter.hpp**
```cpp
class Counter {
    static int count; // declaration – shared by all instances
    int id;

public:
    Counter() : id(++count) {}
    ~Counter() { --count; }

    static int getCount() { return count; } // static member function
    int getId() const { return id; }
};
```

**Counter.cpp**
```cpp
#include "Counter.hpp"
int Counter::count = 0; // definition – must appear in exactly one .cpp file
```

```cpp
Counter a, b, c;

Counter::getCount(); // returns 3 – called on the class, not an instance
a.getCount();        // also valid, but misleading – prefer the class syntax
a.getId();           // returns 1
```

> **Забележка:** Статичните член данни съществуват за целия живот на програмата, независимо дали има създадени инстанции.

Обикновено когато имаме нужда от константа за определен клас, например максимален брой връзки в мрежа, можем да я дефинираме като `static const` или `static constexpr` член данни:

```cpp
class Network {
private:
    void connect(int port = Network::DEFAULT_PORT) {
        if (currentConnections >= MAX_CONNECTIONS) {
            throw std::runtime_error("Too many connections");
        }
        // ... establish connection ...
    }
public:
    static const int MAX_CONNECTIONS = 100; // compile-time constant
    static const int DEFAULT_PORT = 8080;       // runtime constant (needs definition if odr-used)
};
```

### Статични член функции

Статичните член функции **нямат указател `this`** – не могат да достъпват нестатични членове директно. Използват се за операции, свързани с класа, а не с конкретен обект.

```cpp
class MathHelper {
public:
    static int square(int x) { return x * x; }
    static int clamp(int val, int lo, int hi) {
        if (val < lo) return lo;
        if (val > hi) return hi;
        return val;
    }
};
```

```cpp
int a = MathHelper::square(5);        // 25
int b = MathHelper::clamp(15, 0, 10); // 10
```

### Обобщение

| | Нестатичен член | Статичен член |
|:---|:---:|:---:|
| Принадлежи на | Инстанция | Клас |
| Достъп без обект | ❌ | ✅ |
| Достъп до `this` | ✅ | ❌ |
| Споделен между всички обекти | ❌ | ✅ |

### Задача: `basic_string` – статични членове

Добавете към `my_std::basic_string<T>`:

1. **`static size_t instance_count_`** – брои колко живи инстанции на класа съществуват в момента (увеличава се в конструктори, намалява в деструктора). Внимавайте да го отчетете и в move конструктора/оператора.

2. **`static size_t getInstanceCount()`** – връща текущия брой инстанции.

3. **`static constexpr size_t npos = static_cast<size_t>(-1)`** – сентилираща стойност (по модела на `std::string::npos`). Тя се използва за индикация, че дадена операция (напр. търсене) е неуспешна.

4. Добавете метод `find(const T& ch) const`, който връща индекса на първото срещане на `ch`, или `basic_string<T>::npos` при липса.

---

## Приятелски класове и приятелски функции

Ключовата дума `friend` дава на конкретна функция или клас **достъп до частните (`private`) и защитените (`protected`) членове** на даден клас. Приятелството **не е наследяемо**, **не е взаимно** и **не е транзитивно**.

### Приятелска функция

```cpp
class Box {
    int width;
    int height;

public:
    Box(int w, int h) : width(w), height(h) {}

    // grant access to a free function
    friend int area(const Box& b);
};

// not a member, but has access to private fields
int area(const Box& b) {
    return b.width * b.height;
}
```

```cpp
Box b(4, 5);
area(b); // returns 20
```

> Приятелската функция **не е член** на класа – декларира се вътре, но се дефинира извън него (без `Box::`).

### Приятелски клас

```cpp
class Engine {
    int horsepower;
    int rpm;

public:
    Engine(int hp, int r) : horsepower(hp), rpm(r) {}

    // Car gets full access to Engine's private members
    friend class Car;
};

class Car {
    Engine engine;

public:
    Car(int hp, int r) : engine(hp, r) {}

    int getHorsepower() const {
        return engine.horsepower; // direct access to private member
    }
};
```

### Приятелска функция-член на друг клас

Можем да дадем достъп само до **конкретна функция** от друг клас, вместо на целия клас:

```cpp
class Wallet;

class Auditor {
public:
    int checkBalance(const Wallet& w); // forward declaration needed
};

class Wallet {
    int balance;

public:
    Wallet(int b) : balance(b) {}

    // only this specific method of Auditor gets access
    friend int Auditor::checkBalance(const Wallet& w);
};

int Auditor::checkBalance(const Wallet& w) {
    return w.balance;
}
```

### Важни свойства на `friend`

| Свойство | Обяснение |
|:---------|:----------|
| Не е взаимно | Ако `A` е приятел на `B`, `B` не е автоматично приятел на `A` |
| Не е наследяемо | Наследник на приятелски клас не наследява приятелството |
| Не е транзитивно | Ако `A` е приятел на `B` и `B` е приятел на `C`, `A` не е приятел на `C` |
| Нарушава капсулацията | Използвайте `friend` само когато е наистина необходимо |

### Задача:

Дефинирайте функция в неймспейса `my_std` приятелската функция:

```cpp
   template<typename T>
   void swap(basic_string<T>& a, basic_string<T>& b) noexcept;
```

Функцията разменя съдържанието на `a` и `b`. Имплементирайте я с 3 swap операции (без допълнителни алокации).

---

## Контрол над създаването на обекти

C++ позволява фин контрол над това **кога и как** се създават обекти – чрез изрично забраняване или задаване на операции по подразбиране.

### `= default` и `= delete`

- `= default` – казва на компилатора да генерира **стандартната** имплементация.
- `= delete` – **забранява** операцията; опитът за използване е грешка при компилация.

```cpp
class Resource {
    int* data;

public:
    Resource(int n) : data(new int[n]) {}
    ~Resource() { delete[] data; }

    // explicitly ask for compiler-generated move operations
    Resource(Resource&&) noexcept = default;
    Resource& operator=(Resource&&) noexcept = default;

    // disable copy – this object must not be duplicated
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
};
```

```cpp
Resource a(5);
Resource b = std::move(a); // OK – move is allowed
// Resource c = a;         // ERROR – copy is deleted
```

### Само за стека (забрана на heap-алокация)

Чрез изтриване на `operator new` обектите могат да се създават **само на стека**:

```cpp
class StackOnly {
public:
    StackOnly() = default;

    void* operator new(size_t) = delete;
    void* operator new[](size_t) = delete;
};
```

```cpp
StackOnly a;              // OK – on the stack
// StackOnly* p = new StackOnly; // ERROR – operator new is deleted
```

### Само за heap-а (забрана на стек-алокация)

Чрез правене на деструктора `private` или `protected` обектите не могат да се създадат на стека (компилаторът не може да генерира кода за автоматичното им унищожаване). Вместо това се предоставя статичен фабричен метод и метод за унищожаване:

```cpp
class HeapOnly {
    int value;

    ~HeapOnly() = default; // private destructor

public:
    HeapOnly(int v) : value(v) {}

    static HeapOnly* create(int v) { return new HeapOnly(v); }
    void destroy() { delete this; }
};
```

```cpp
// HeapOnly a(5);          // ERROR – destructor is inaccessible
HeapOnly* p = HeapOnly::create(5);
p->destroy();
```

### Забрана на конструктора по подразбиране

Полезно, когато обектът **задължително изисква аргументи** при създаването:

```cpp
class Connection {
    int port;

public:
    Connection() = delete; // must provide a port

    explicit Connection(int p) : port(p) {}
};
```

```cpp
// Connection c;       // ERROR – default constructor is deleted
Connection c(8080);    // OK
```

### `explicit` – забрана на неявни преобразувания

Ключовата дума `explicit` пред конструктор (или оператор за преобразуване) **забранява имплицитното му извикване**:

```cpp
class Angle {
    double degrees;

public:
    explicit Angle(double d) : degrees(d) {}
};

void rotate(Angle a) { /* ... */ }
```

```cpp
rotate(Angle(90.0)); // OK – explicit construction
// rotate(90.0);     // ERROR – implicit conversion is forbidden
```

### Задача: `basic_string` – контрол на създаването

Приложете следните ограничения и улеснения към `my_std::basic_string<T>`:

- Направете конструктора `basic_string(const T* ptr, size_t len)` **`explicit`**. Какво предотвратява това?
- Създайте клас `StringView<T>` (прост, немутиращ изглед към `basic_string<T>`):
   - Може да се създава само от `const basic_string<T>&` – **забранете** конструиране от rvalue (`StringView(basic_string<T>&&) = delete`).
   - Предоставя `size()`, `operator[]` и `begin()`/`end()` (само const версии).
   - **Забранете** копиращото присвояване (`= delete`), но оставете конструктора за копиране.
  
Целта е `StringView` да може да се създава от съществуващ `basic_string`, но да не може да се копира или да се създава от временен обект, който може да бъде унищожен.

---

## Шаблони за дизайн

Шаблоните за дизайн (design patterns) са **утвърдени решения на често срещани проблеми** в обектно-ориентираното проектиране. Те не са готов код, а архитектурни идиоми.

[Всички шаблони за дизайн](https://refactoring.guru/design-patterns) \
[Оригиналната книга на създателите на шаблоните](https://github.com/deepakkum21/Books/blob/master/Design%20Patterns%20-%20Elements%20of%20Reusable%20Object%20Oriented%20Software%20-%20GOF.pdf)

### Singleton

**Цел:** Гарантира, че даден клас има **точно една инстанция** и предоставя глобална точка за достъп до нея.

**Кога се използва:** Конфигурационни обекти, логери, мениджъри на ресурси – навсякъде, където споделеното глобално състояние е оправдано.

Имплементация в [`code/design_patterns/LoggerSingleton.hpp`](code/design_patterns/LoggerSingleton.hpp):

```cpp
Logger::getInstance().log("App started");
Logger::getInstance().log("Loading config");
Logger::getInstance().getLogCount(); // 2
```

> **Предимство на `static` локална променлива:** Инициализира се мързеливо (lazy) – само при първо извикване, и е унищожена при край на програмата.

### Builder

**Цел:** Разделя **конструирането** на сложен обект от неговото **представяне**, като позволява същият процес на конструиране да създава различни представяния. **Обектът се изгражда стъпка по стъпка**.

**Кога се използва:** Когато обектът има много незадължителни полета или сложна инициализационна логика, която е тромаво да се изрази само с конструктори.

> Чувал съм, че и по интервюта се кефят на точно този шаблон, защото е един от най-приложимите в продакшън код и не е особено сложен за разбиране.

Имплементация в [`code/design_patterns/ServerConfigBuilder.hpp`](code/design_patterns/ServerConfigBuilder.hpp):

```cpp
ServerConfig cfg = ServerConfigBuilder()
    .setHost("example.com")
    .enableTls()
    .setTimeout(60)
    .setMaxConnections(50)
    .build();

// cfg.host           → "example.com"
// cfg.port           → 443  (set by enableTls)
// cfg.useTls         → true
// cfg.timeout        → 60
// cfg.maxConnections → 50
```