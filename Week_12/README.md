# Виртуални методи и полиморфизъм

## Съдържание

- [Виртуални методи и полиморфизъм](#виртуални-методи-и-полиморфизъм)
  - [Съдържание](#съдържание)
  - [SOLID – L, I и D принципа](#solid--l-i-и-d-принципа)
    - [L – Принцип на заместването на Лисков (LSP)](#l--принцип-на-заместването-на-лисков-lsp)
    - [I – Принцип за сегрегация на интерфейсите (ISP)](#i--принцип-за-сегрегация-на-интерфейсите-isp)
    - [D – Принцип на инверсия на зависимостите (DIP)](#d--принцип-на-инверсия-на-зависимостите-dip)
  - [Проблемът без виртуални методи](#проблемът-без-виртуални-методи)
  - [Виртуални методи](#виртуални-методи)
    - [Ключовата дума `virtual`](#ключовата-дума-virtual)
    - [Механизмът зад виртуалните методи – vtable](#механизмът-зад-виртуалните-методи--vtable)
      - [Визуализация на vtable с Clang](#визуализация-на-vtable-с-clang)
    - [Ключовата дума `override`](#ключовата-дума-override)
    - [Ключовата дума `final`](#ключовата-дума-final)
  - [Чисто виртуални методи и абстрактни класове](#чисто-виртуални-методи-и-абстрактни-класове)
    - [Чисто виртуален метод (`= 0`)](#чисто-виртуален-метод--0)
    - [Абстрактен клас](#абстрактен-клас)
    - [Конкретен клас](#конкретен-клас)
  - [Виртуален деструктор](#виртуален-деструктор)
  - [`dynamic_cast`](#dynamic_cast)
    - [Downcast – от базов към производен](#downcast--от-базов-към-производен)
    - [Upcast – от производен към базов](#upcast--от-производен-към-базов)
    - [Проверка на тип в хетерогенна колекция](#проверка-на-тип-в-хетерогенна-колекция)
    - [`dynamic_cast` срещу `static_cast`](#dynamic_cast-срещу-static_cast)
  - [Полиморфизъм](#полиморфизъм)
    - [Какво е полиморфизъм](#какво-е-полиморфизъм)
    - [Работа с масив от указатели към базов клас](#работа-с-масив-от-указатели-към-базов-клас)
  - [Задачи](#задачи)

---

## SOLID – L, I и D принципа

---

### L – Принцип на заместването на Лисков (LSP)

> *Ако `S` е подтип на `T`, тогава обектите от тип `T` могат да бъдат заместени с обекти от тип `S`, без да се нарушава коректността на програмата.*

Казано по-просто: производният клас трябва да може да се използва навсякъде, където се очаква базовият, **без изненади**.

**Нарушение на LSP – класическият пример с правоъгълник и квадрат:**

```cpp
class Rectangle {
protected:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    virtual void set_width(double w)  { width = w; }
    virtual void set_height(double h) { height = h; }
    double area() const { return width * height; }
};

// Square IS-A Rectangle mathematically, but NOT behaviourally
class Square : public Rectangle {
public:
    Square(double side) : Rectangle(side, side) {}

    // Must keep both sides equal, so overriding one setter forces the other
    void set_width(double w) override  { width = height = w; }
    void set_height(double h) override { width = height = h; }
};

void make_5x10(Rectangle& r) {
    r.set_width(5);
    r.set_height(10);
    // Expected area: 50. With Square it is 100 – LSP violated!
}
```

`Square` нарушава LSP защото подменя инварианта на `Rectangle` (`width` и `height` са независими). Функция, написана да работи с `Rectangle`, дава грешен резултат при `Square`.

**Правилното решение** е да не се наследява в тази посока – вместо това и двата класа могат да наследят обща абстракция `Shape`:

```cpp
class Shape {
public:
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Rectangle : public Shape { /* ... */ };
class Square    : public Shape { /* ... */ };
```

> **Правило:** Ако не можете да заместите базовия клас с производния без да счупите нещо, йерархията нарушава LSP и трябва да се преосмисли.

---

### I – Принцип за сегрегация на интерфейсите (ISP)

> *Клиентите не трябва да бъдат принуждавани да зависят от интерфейси, които не използват.*

По-добре е да имате много малки, специализирани интерфейса, отколкото един голям общ.

**Нарушение на ISP – "дебел" интерфейс:**

```cpp
class IMultifunction {
public:
    virtual void print()  = 0;
    virtual void scan()   = 0;
    virtual void fax()    = 0;
    virtual void staple() = 0;
    virtual ~IMultifunction() = default;
};

// A simple printer is forced to implement scan, fax and staple
class SimplePrinter : public IMultifunction {
public:
    void print()  override { /* actual printing */ }
    void scan()   override { /* not supported – what to do here? */ }
    void fax()    override { /* not supported */ }
    void staple() override { /* not supported */ }
};
```

`SimplePrinter` е принуден да имплементира методи, за които няма смисъл. Това е нарушение на ISP.

**Правилното решение – разделяне на интерфейсите:**

```cpp
class IPrintable {
public:
    virtual void print() = 0;
    virtual ~IPrintable() = default;
};

class IScannable {
public:
    virtual void scan() = 0;
    virtual ~IScannable() = default;
};

class IFaxable {
public:
    virtual void fax() = 0;
    virtual ~IFaxable() = default;
};

// Simple printer only needs to print
class SimplePrinter : public IPrintable {
public:
    void print() override { /* actual printing */ }
};

// High-end device implements multiple interfaces
class OfficeMachine : public IPrintable, public IScannable, public IFaxable {
public:
    void print() override { /* ... */ }
    void scan()  override { /* ... */ }
    void fax()   override { /* ... */ }
};
```

> **Правило:** Ако класовете трябва да имплементират методи, за които няма смисъл (или ги оставят празни с `{}` / хвърлят изключение), интерфейсът е прекалено голям – разделете го.

---

### D – Принцип на инверсия на зависимостите (DIP)

> *Модулите от високо ниво не трябва да зависят от модули от ниско ниво. И двете трябва да зависят от абстракции. Абстракциите не трябва да зависят от детайлите – детайлите трябва да зависят от абстракциите.*

**Нарушение на DIP – зависимост от конкретна реализация:**

```cpp
class FileLogger {
public:
    void log(const char* msg) { /* writes to file */ }
};

// High-level class depends directly on the low-level FileLogger
class OrderProcessor {
    FileLogger logger;   // tightly coupled – cannot swap to a different logger
public:
    void process() {
        logger.log("Processing order...");
        /* business logic */
    }
};
```

`OrderProcessor` е здраво свързан с `FileLogger`. За да се смени с `ConsoleLogger`, трябва да се промени `OrderProcessor`.

**Правилното решение – зависимост от абстракция:**

```cpp
class ILogger {
public:
    virtual void log(const char* msg) = 0;
    virtual ~ILogger() = default;
};

class FileLogger : public ILogger {
public:
    void log(const char* msg) override { /* writes to file */ }
};

class ConsoleLogger : public ILogger {
public:
    void log(const char* msg) override { /* writes to stdout */ }
};

// High-level class depends on the abstraction, not the concrete type
class OrderProcessor {
    ILogger& logger;   // depends on interface, not implementation
public:
    OrderProcessor(ILogger& l) : logger(l) {}

    void process() {
        logger.log("Processing order...");
        /* business logic */
    }
};

// At the call site we decide which concrete logger to use
ConsoleLogger console_log;
OrderProcessor proc(console_log);
proc.process();
```

> **Правило:** Когато клас от високо ниво трябва да ползва функционалност от ниско ниво, въведете интерфейс (абстрактен клас) между тях. Зависи от интерфейса, не от конкретния клас.

---

## Проблемът без виртуални методи

От предишната тема знаем, че производен обект може да се третира като базов (upcasting). Но когато викаме метод през указател или референция към базовия клас, компилаторът – **без виртуални методи** – решава **по тип на указателя**, не по реалния тип на обекта:

```cpp
class Shape {
public:
    void describe() const {
        // base version – always called when using a Shape pointer/reference
    }
    double area() const { return 0.0; }
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}

    // Hides Shape::describe and Shape::area, but does NOT override them
    void describe() const {
        // circle-specific description
    }
    double area() const { return 3.14159 * radius * radius; }
};

Shape*  ptr  = new Circle(5.0);
Shape&  ref  = *ptr;

ptr->describe();   // calls Shape::describe() – WRONG, we wanted Circle::describe()
ptr->area();       // calls Shape::area() → 0.0 – WRONG
ref.describe();    // calls Shape::describe() – still wrong
```

Компилаторът вижда `Shape*` и извиква `Shape::describe`, независимо че зад указателя стои `Circle`. Това се нарича **статичен dispatch** (решен по време на компилация).

За да получим **правилното поведение** – извикване на версията на реалния обект – трябва виртуален метод.

---

## Виртуални методи

### Ключовата дума `virtual`

Добавянето на `virtual` пред метод в базовия клас инструктира компилатора да използва **динамичен dispatch** – решението кой метод да се извика се взема по **реалния тип на обекта** по време на изпълнение:

```cpp
class Shape {
public:
    // virtual enables dynamic dispatch – the actual object's method is called
    virtual void describe() const {
        // base version
    }
    virtual double area() const { return 0.0; }

    // Always declare virtual destructor in a polymorphic base class (explained later)
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}

    void describe() const override {   // override is good practice (C++11)
        // circle-specific description
    }
    double area() const override { return 3.14159 * radius * radius; }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}

    void describe() const override {
        // rectangle-specific description
    }
    double area() const override { return width * height; }
};

Shape* ptr = new Circle(5.0);
ptr->describe();        // calls Circle::describe() – correct!
ptr->area();            // calls Circle::area() – correct!

delete ptr;
ptr = new Rectangle(3.0, 4.0);
ptr->describe();        // calls Rectangle::describe() – correct!
ptr->area();            // 12.0 – correct!
delete ptr;
```

Сега `ptr->describe()` вика версията на реалния обект зад указателя – **не** версията на типа на указателя.

---

### Механизмът зад виртуалните методи – vtable

[Информацията за това съм я взимал предимно от тук](https://peter0x44.github.io/posts/vtables-itanium-abi/)

Когато клас съдържа поне един виртуален метод, компилаторът добавя към всеки обект скрит указател (`vptr`) към **таблица с виртуални методи** (`vtable`). `vtable`-ът е масив от указатели към функции – по един за всеки виртуален метод:

```
Shape vtable:
  [0] → Shape::describe
  [1] → Shape::area
  [2] → Shape::~Shape

Circle vtable:
  [0] → Circle::describe   ← overridden
  [1] → Circle::area       ← overridden
  [2] → Shape::~Shape      ← not overridden (but virtual destructor chain works)

Memory layout of a Circle object:
  +----------+
  |  vptr    | → points to Circle's vtable
  +----------+
  |  radius  |
  +----------+
```

При извикване на `ptr->describe()` компилаторът генерира:
1. Прочети `vptr` от обекта.
2. Влез в `vtable` на индекс 0.
3. Извикай функцията там.

Затова когато `ptr` сочи към `Circle`, се извиква `Circle::describe`, а не `Shape::describe`.

> **Цена:** Всеки обект носи допълнителен `vptr` (обикновено 8 байта на 64-bit системи). Всяко виртуално извикване минава през допълнително indirection. На практика цената е пренебрежима за повечето приложения.

#### Визуализация на vtable с Clang

Clang може да изпечата реалния vtable layout на вашите класове директно от командния ред. Полезно е за проверка на разбирането и за дебъгване на по-сложни йерархии.

**Команда:**

```bash
clang++ -Xclang -fdump-vtable-layouts -std=c++17 -c file.cpp
```

- `-Xclang -fdump-vtable-layouts` — предава вътрешния флаг на Clang frontend, който отпечатва vtable-а за всеки клас с виртуални методи.
- `-std=c++17` — стандартът (заменете с `-std=c++14` или `-std=c++20` според нуждите).
- `-c` — само компилация, без линкване; изходът е само vtable dump в stdout/stderr.

---

### Ключовата дума `override`

`override` (C++11) казва на компилатора: *"Тук имам намерение да предефинирам виртуален метод от базовия клас."* Ако такъв метод не съществува, компилаторът сигнализира грешка – улавя правописни грешки и несъответствия в подписа:

```cpp
class Base {
public:
    virtual void process(int x) {}
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void process(int x) override {}   // OK – correctly overrides Base::process(int)

    // void process(double x) override {} // COMPILE ERROR – no such virtual in Base
    // void procces(int x)    override {} // COMPILE ERROR – typo caught at compile time
};
```

> **Правило:** Винаги пишете `override` при предефиниране. Без него можете да скриете метод по невнимание вместо да го предефинирате.

---

### Ключовата дума `final`

`final` (C++11) предотвратява по-нататъшно предефиниране на виртуален метод, или наследяване от клас:

```cpp
class Base {
public:
    virtual void render() {}
    virtual ~Base() = default;
};

class Optimized : public Base {
public:
    // No further class may override render() after this point
    void render() final {}
};

class Further : public Optimized {
public:
    // void render() override {}   // COMPILE ERROR – render() is final in Optimized
};

// Prevents any class from inheriting from Sealed
class Sealed final : public Base {
public:
    void render() override {}
};

// class Child : public Sealed {}; // COMPILE ERROR – Sealed is final
```

> **Употреба:** `final` на метод подсказва на компилатора да девиртуализира извикванията (оптимизация). `final` на клас е рядко – само когато наследяването е концептуално невъзможно.

---

## Чисто виртуални методи и абстрактни класове

### Чисто виртуален метод (`= 0`)

Понякога базовият клас не може да предостави смислена имплементация – просто дефинира **интерфейс**, който производните класове трябва да изпълнят. За това служи **чисто виртуалният метод**:

```cpp
class Shape {
public:
    // Pure virtual: no implementation here, derived classes MUST provide one
    virtual double area()      const = 0;
    virtual double perimeter() const = 0;
    virtual void   describe()  const = 0;

    virtual ~Shape() = default;
};
```

Синтаксисът `= 0` означава: *"Нямам имплементация. Производният клас е задължен да я предостави."*

---

### Абстрактен клас

Клас с поне един чисто виртуален метод е **абстрактен**. Не може да се създаде обект от него директно:

```cpp
// Shape shape;              // COMPILE ERROR – Shape is abstract
// Shape* p = new Shape();   // COMPILE ERROR – Shape is abstract

Shape* p = new Circle(5.0);  // OK – Circle is concrete (implements all pure virtuals)
```

Абстрактните класове служат за дефиниране на **интерфейс** (набор от операции), без да налагат конкретна реализация. Те са инструментът за изграждане на полиморфни йерархии.

---

### Конкретен клас

Производен клас, който имплементира **всички** наследени чисто виртуални методи, е **конкретен** и може да бъде инстанциран:

```cpp
class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}

    // Must implement all pure virtuals from Shape
    double area()      const override { return 3.14159 * radius * radius; }
    double perimeter() const override { return 2 * 3.14159 * radius; }
    void   describe()  const override { /* prints circle info */ }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}

    double area()      const override { return width * height; }
    double perimeter() const override { return 2 * (width + height); }
    void   describe()  const override { /* prints rectangle info */ }
};

// Both are concrete – can be instantiated
Circle    c(5.0);
Rectangle r(3.0, 4.0);
```

Ако производен клас **не** имплементира всички чисто виртуални методи, той остава абстрактен:

```cpp
class PartialShape : public Shape {
public:
    double area() const override { return 0.0; }
    // perimeter() and describe() are still pure virtual here
};

// PartialShape ps;  // COMPILE ERROR – still abstract
```

---

## Виртуален деструктор

Това е едно от **най-важните правила** при полиморфни йерархии.

**Проблемът без виртуален деструктор:**

```cpp
class Base {
public:
    Base()  { /* allocates some resource */ }
    ~Base() { /* frees base resource – NOT virtual! */ }
};

class Derived : public Base {
    int* data;
public:
    Derived() : data(new int[100]) {}
    ~Derived() { delete[] data; }   // must free data
};

Base* ptr = new Derived();
delete ptr;   // UNDEFINED BEHAVIOUR: calls ~Base() only, ~Derived() never runs
              // data is leaked!
```

Без `virtual` деструктор, `delete ptr` вика само `~Base()`. `~Derived()` не се извиква – паметта за `data` изтича.

**Правилното решение:**

```cpp
class Base {
public:
    Base()  {}
    virtual ~Base() {}   // virtual: ensures the correct destructor chain is called
};

class Derived : public Base {
    int* data;
public:
    Derived() : data(new int[100]) {}
    ~Derived() override { delete[] data; }   // now called correctly
};

Base* ptr = new Derived();
delete ptr;   // calls ~Derived() first, then ~Base() automatically – correct!
```

Когато деструкторът е виртуален, `delete ptr` минава през vtable и вика `~Derived()`, след което автоматично се вика `~Base()`.

> **Правило:** Ако клас е предназначен да се наследява и обектите му ще се изтриват през указател към базовия клас, деструкторът **задължително** трябва да е `virtual`. В абстрактните класове използвайте `virtual ~Base() = default`.

---

## `dynamic_cast`

`dynamic_cast` е оператор за **проверено преобразуване по йерархията** по време на изпълнение. Работи само с полиморфни класове (такива с поне един `virtual` метод). За разлика от `static_cast`, той **проверява** дали преобразуването е валидно и не води до неопределено поведение при грешен тип.

Ще използваме следната йерархия за всички примери:

```cpp
class Animal {
public:
    virtual void speak() const = 0;
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak()  const override { /* "Woof" */ }
    void fetch()  const { /* Dog-specific */ }
};

class Cat : public Animal {
public:
    void speak()  const override { /* "Meow" */ }
    void purr()   const { /* Cat-specific */ }
};
```

---

### Downcast – от базов към производен

Най-честата употреба: имаме `Animal*` и искаме да достъпим метод, специфичен за `Dog`.

**С указател** – при неуспех връща `nullptr`:

```cpp
Animal* a = new Dog();

Dog* d = dynamic_cast<Dog*>(a);
if (d != nullptr) {
    d->fetch();   // safe – a really is a Dog
}

Cat* c = dynamic_cast<Cat*>(a);   // returns nullptr – a is not a Cat
if (c == nullptr) {
    // cast failed, handle gracefully
}
```

**С референция** – при неуспех хвърля `std::bad_cast`:

```cpp
#include <typeinfo>   // for std::bad_cast

Animal& ref = *a;

try {
    Dog& d = dynamic_cast<Dog&>(ref);
    d.fetch();   // safe
} catch (const std::bad_cast&) {
    // ref is not a Dog
}
```

---

### Upcast – от производен към базов

Upcast с `dynamic_cast` винаги успява и е безопасен – производният обект съдържа базовата си част:

```cpp
Dog* d = new Dog();

Animal* a = dynamic_cast<Animal*>(d);   // always succeeds
a->speak();                             // calls Dog::speak() via virtual dispatch

// Upcast is implicit with public inheritance, so this is equivalent:
Animal* a2 = d;   // same effect, no cast needed
```

> На практика upcast не изисква `dynamic_cast` – имплицитното преобразуване е достатъчно и по-четимо. `dynamic_cast` е полезен основно при downcast.

---

### Проверка на тип в хетерогенна колекция

Типичен сценарий: масив от `Animal*` с различни реални типове. `dynamic_cast` позволява да разпознаем конкретния тип и да извикаме специфичен метод:

```cpp
Animal* zoo[3] = { new Dog(), new Cat(), new Dog() };

for (int i = 0; i < 3; i++) {
    zoo[i]->speak();   // virtual – works for all

    if (Dog* d = dynamic_cast<Dog*>(zoo[i])) {
        d->fetch();    // only for Dogs
    } else if (Cat* c = dynamic_cast<Cat*>(zoo[i])) {
        c->purr();     // only for Cats
    }
}

for (int i = 0; i < 3; i++) delete zoo[i];
```

Забележете `if (Dog* d = dynamic_cast<Dog*>(...))` – декларацията на указателя директно в условието е идиоматичен C++ начин да се комбинират cast и проверка в един ред.

---

### `dynamic_cast` срещу `static_cast`

```cpp
Animal* a = new Dog();

Dog* safe   = dynamic_cast<Dog*>(a);   // checked at runtime – nullptr if wrong
Dog* unsafe = static_cast<Dog*>(a);   // no check – UB if a is not actually a Dog

Cat* fail   = dynamic_cast<Cat*>(a);   // nullptr – correct, no crash
// Cat* ub  = static_cast<Cat*>(a);   // compiles, but UB at runtime
```

> **Правило:** При downcast винаги използвайте `dynamic_cast`. Честата нужда от него е сигнал, че в базовия клас липсва виртуален метод – по-добре добавете метода там, отколкото да разпознавате типове ръчно.

---

## Полиморфизъм

### Какво е полиморфизъм

**Полиморфизъм** (от гръцки: много форми) е способността на различни типове да се третират по еднакъв начин чрез общ интерфейс, като всеки тип реагира по свой специфичен начин.

В C++ runtime полиморфизмът се постига чрез **виртуални методи** и **указатели/референции към базовия клас**:

```cpp
// Same call – different behaviour depending on the actual type
void print_area(const Shape& s) {
    s.describe();   // calls the correct describe() for Circle, Rectangle, etc.
    // s.area() would also dispatch to the correct override
}

Circle    c(5.0);
Rectangle r(3.0, 4.0);

print_area(c);   // calls Circle::describe()
print_area(r);   // calls Rectangle::describe()
```

Функцията `print_area` не знае нищо за `Circle` или `Rectangle` – тя работи само с `Shape`. Конкретното поведение се определя по време на изпълнение.

---

### Работа с масив от указатели към базов клас

Класически пример за полиморфизъм: хетерогенна колекция от обекти, обработени с един цикъл:

```cpp
class Shape {
public:
    virtual double area()     const = 0;
    virtual void   describe() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area()     const override { return 3.14159 * radius * radius; }
    void   describe() const override { /* prints "Circle r=..." */ }
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h) : w(w), h(h) {}
    double area()     const override { return w * h; }
    void   describe() const override { /* prints "Rectangle w=... h=..." */ }
};

class Triangle : public Shape {
    double base, height;
public:
    Triangle(double b, double hh) : base(b), height(hh) {}
    double area()     const override { return 0.5 * base * height; }
    void   describe() const override { /* prints "Triangle b=... h=..." */ }
};

// Heterogeneous array – circles, rectangles and triangles side by side
const int N = 4;
Shape* shapes[N] = {
    new Circle(5.0),
    new Rectangle(3.0, 4.0),
    new Triangle(6.0, 8.0),
    new Circle(2.5)
};

// Single loop handles all types uniformly – this is polymorphism
double total = 0.0;
for (int i = 0; i < N; i++) {
    shapes[i]->describe();
    total += shapes[i]->area();
}
// total now holds the sum of all areas

// Always clean up
for (int i = 0; i < N; i++) {
    delete shapes[i];   // virtual destructor ensures correct cleanup
}
```

Добавянето на нова форма (напр. `Pentagon`) не изисква промяна на цикъла – само нов клас, наследяващ `Shape`. Това е **отворено/затворено** (Open/Closed) поведение – системата е отворена за разширение, затворена за модификация.

---

## Задачи

**Задача 1 – Форми**

Дефинирайте абстрактен клас `Shape` с чисто виртуални методи `area()`, `perimeter()` и `describe()`. Имплементирайте `Circle`, `Rectangle` и `Triangle` като конкретни наследници. Напишете функция `print_summary(Shape* shapes[], int n)`, която обхожда масива и отпечатва описанието и лице на всяка форма, без да знае конкретните типове.

---

**Задача 2 – Виртуален деструктор**

Дефинирайте клас `Resource` с конструктор, който симулира заделяне на ресурс (отпечатва `"acquiring"`), и деструктор, който симулира освобождаване (`"releasing"`). Направете `Resource` наследник на прост базов клас `Handle`. Тествайте с `Handle* h = new Resource(); delete h;` – първо без виртуален деструктор (наблюдавайте проблема), после с виртуален деструктор (наблюдавайте коректното поведение).

---

**Задача 3 – DIP в практиката**

Дефинирайте интерфейс `IStorage` с методи `save(const char* data)` и `load()`. Имплементирайте `FileStorage` и `MemoryStorage`. Напишете клас `DataManager`, чийто конструктор приема `IStorage&` и работи единствено с интерфейса. Тествайте, че `DataManager` работи правилно и с двете реализации на `IStorage`, без промяна в собствения си код.

---

**Задача 4 – `dynamic_cast`**

Използвайте йерархията от Задача 1. Създайте масив от `Shape*`, съдържащ смесица от `Circle`, `Rectangle` и `Triangle`. Напишете функция, която с `dynamic_cast` проверява дали всяка форма е `Circle`, и ако е – отпечатва допълнителна специфична информация (напр. диаметъра). За останалите форми печата само `describe()`.

---
