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
  - [Прототипен шаблон – `clone()`](#прототипен-шаблон--clone)
    - [Ковариантни типове на връщане](#ковариантни-типове-на-връщане)
    - [Връщане на производен указател срещу базов](#връщане-на-производен-указател-срещу-базов)
    - [Връзката с принципа на Лисков](#връзката-с-принципа-на-лисков)
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

> **Правило:** Винаги пишете `override` при предефиниране. Без него можете да  скриете метод по невнимание вместо да го предефинирате.

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

## Прототипен шаблон – `clone()`

[Refactoring Guru](https://refactoring.guru/design-patterns/prototype)

**Проблемът:** имате указател към базов клас и искате да направите точно копие на обекта зад него – без да знаете конкретния му тип.

```cpp
Shape* original = new Circle(5.0);

// Wrong – copies only the Shape slice (object slicing):
Shape copy = *original;

// Wrong – we don't know the concrete type here:
// Circle* c = new Circle(*original);   // can't do this through a Shape*
```

**Решението** е всеки клас да умее да копира себе си чрез виртуален метод `clone()`. Това е **Прототипният шаблон (Prototype Pattern)**:

```cpp
class Shape {
public:
    virtual Shape* clone() const = 0;   // each derived class returns a heap copy of itself
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}

    Shape* clone() const override { return new Circle(*this); }
    double area()  const override { return 3.14159 * radius * radius; }
};

class Rectangle : public Shape {
    double w, h;
public:
    Rectangle(double w, double h) : w(w), h(h) {}

    Shape* clone() const override { return new Rectangle(*this); }
    double area()  const override { return w * h; }
};
```

Сега можем да копираме обект без да знаем конкретния му тип:

```cpp
Shape* original = new Circle(5.0);
Shape* copy     = original->clone();   // returns a new Circle, seen as Shape*

// Both are independent heap objects – modifying one does not affect the other
```

---

### Ковариантни типове на връщане

C++ позволява специално изключение от правилото, че виртуален метод трябва да бъде предефиниран с **точно същия** тип на връщане. Ако базовият метод връща `Base*` (или `Base&`), производният може да върне `Derived*` (или `Derived&`), при условие, че `Derived` наследява `Base`. Тази функция се казва **ковариантен тип на връщане (covariant return type)**:

```cpp
class Shape {
public:
    virtual Shape* clone() const = 0;   // returns Shape*
    virtual ~Shape() = default;
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}

    // Covariant return type: Circle* instead of Shape* – still a valid override
    Circle* clone() const override { return new Circle(*this); }
};
```

Компилаторът приема `Circle*` като валидно предефиниране на `Shape* clone()`, защото `Circle` е производен от `Shape` – т.е. `Circle*` е специализация на `Shape*`.

---

### Връщане на производен указател срещу базов

Разликата е видима на мястото на извикване:

```cpp
// ---- Scenario A: clone() returns Shape* (base pointer) ----

Shape* a = new Circle(5.0);
Shape* copy_a = a->clone();   // type is Shape* – only Shape interface visible

// To call Circle-specific method we must cast:
Circle* c = dynamic_cast<Circle*>(copy_a);
if (c) { /* circle-specific work */ }


// ---- Scenario B: clone() returns Circle* (covariant/derived pointer) ----

Circle original(5.0);
Circle* copy_b = original.clone();   // type is Circle* – no cast needed!
// copy_b->radius_times_two();       // can call Circle-specific methods directly
```

Когато извиквате `clone()` директно **върху производен тип** (не чрез базов указател), ковариантният тип позволява да получите обратно производния тип без никакво cast. Когато извиквате `clone()` чрез `Shape*`, компилаторът вижда само `Shape*` – ковариантността се "губи" в динамичния dispatch, но поведението е правилно.

```cpp
// Through a base pointer – you get Shape* back (base interface only):
Shape* s = new Circle(5.0);
Shape* cloned_s = s->clone();   // Shape*, not Circle*

// Through a concrete type – you get Circle* back (full interface):
Circle c(5.0);
Circle* cloned_c = c.clone();   // Circle* directly, no cast
```

Обобщено:

| Извикване чрез | Тип на резултата | Cast нужен? |
|---|---|---|
| `Shape*` или `Shape&` | `Shape*` | Да, ако трябват производни методи |
| `Circle` или `Circle*` директно | `Circle*` | Не |

---

### Връзката с принципа на Лисков

Ковариантните типове на връщане **са разрешени именно защото спазват LSP**.

LSP изисква: навсякъде, където се очаква `Shape*`, може да се подаде `Circle*` без изненади – тъй като `Circle` е `Shape`. Следователно метод, обявен да връща `Shape*`, може безопасно да върне `Circle*` вместо това: извикващият код очаква `Shape*` и точно това получава (или нещо по-специфично, което е напълно съвместимо).

```cpp
// The caller expects Shape* – getting Circle* is fine, it IS-A Shape*
Shape* result = some_shape->clone();   // always safe, LSP holds
```

**Обратното – контравариантен тип – би нарушило LSP** и затова не е разрешено в C++:

```cpp
class SpecialCircle : public Circle {
public:
    // COMPILE ERROR – Shape* is LESS derived than Circle*:
    // returning a base pointer from an override of a method that returns a derived pointer
    // would mean SpecialCircle::clone() can return something that is NOT a Circle,
    // breaking every caller that expects a Circle* from a Circle's clone().
    Shape* clone() const override { return new Circle(1.0); }
};
```

Ако `SpecialCircle::clone()` можеше да върне `Shape*` вместо `Circle*`, кодът, който извиква `clone()` на `Circle*` и очаква `Circle*` обратно, би получил нещо несъвместимо. Това директно нарушава LSP.

> **Правило за запомняне:**
> - **Ковариантен тип** (по-производен при override) → разрешен → спазва LSP.
> - **Контравариантен тип** (по-базов при override) → забранен → нарушава LSP.
>
> Ковариантните типове на връщане са C++ начинът да кажете: *"Аз правя всичко, което базовият клас обещава, и дори малко повече."*

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

**Задача 5 – Симулатор на аудио верига**

Имате задача да напишете симулатор на верига от звукови ефекти – като педалборда на китарист. Аудио сигнал (просто `float` стойност) минава последователно през поредица от ефекти, всеки от които го трансформира по свой начин. Един "логър" наблюдава какво се случва във веригата – някои логъри извеждат всяка стъпка, други мълчат и трупат статистика.

**Контекст**

Веригата съдържа различни ефекти: `Overdrive` (изкривяване), `Reverb` (ехо/зала), `Delay` (повторение) и `Chorus` (удебеляване на звука). Всеки ефект има параметри, описание и умее да обработи сигнал. `Overdrive`-ът е специален – освен стандартната си обработка, той може да бъде "увеличен" с метод `boost()`, който за следващото извикване на `apply` удвоява изкривяването. Този метод **не** съществува в базовия клас.

---

**(а) Интерфейс `ISignalLogger`**

Дефинирайте абстрактен клас `ISignalLogger` с:

- Чисто виртуален метод `void log_stage(const char* effect_name, float input, float output)` – извиква се след всяка обработка от един ефект.
- Чисто виртуален метод `void log_chain_result(float final_signal)` – извиква се веднъж, в края на цялата верига.
- Виртуален деструктор.

Имплементирайте две конкретни реализации:

**`ConsoleSignalLogger`**
- Няма състояние.
- `log_stage` отпечатва на `std::cout` ред от вида `[effect_name] in=<input> -> out=<output>`.
- `log_chain_result` отпечатва ред от вида `=> final = <final_signal>`.

**`StatsSignalLogger`** – натрупва статистика, без да печата нищо по време на обработката.
- Член-данни:
  - `unsigned m_stage_count` – брой обработени стъпки.
  - `unsigned m_chain_count` – брой завършени вериги.
  - `float m_max_abs_output` – най-голямата абсолютна стойност на изходен сигнал, която е виждал.
  - `float m_last_final` – резултат от последната верига.
- `log_stage` увеличава `m_stage_count` и обновява `m_max_abs_output`, ако `|output|` е по-голямо от текущото.
- `log_chain_result` записва `final_signal` в `m_last_final` и инкрементира `m_chain_count`.
- Допълнителни public методи (които **не** са в интерфейса):
  - `unsigned stage_count() const`
  - `unsigned chain_count() const`
  - `float max_abs_output() const`
  - `float last_final() const`
  - `void print_report() const` – отпечатва обобщение, използва се ръчно в `main`.

*Целта на двата логъра е да покажат, че имплементации на един и същ интерфейс могат да правят коренно различни неща – единият извежда в реално време, другият мълчи и трупа данни за по-късно. И в двата случая `Pedalboard` ги използва по абсолютно еднакъв начин.*

---

**(б) Базов клас `Effect`**

Дефинирайте абстрактен клас `Effect` с:

- Чисто виртуален метод `float apply(float signal)` – обработва сигнала и връща резултата.
- Чисто виртуален метод `const char* name() const` – връща името на ефекта (напр. `"Overdrive"`).
- Чисто виртуален метод `void describe() const` – отпечатва на `std::cout` описание във формат: `<name>: <параметри и кратко обяснение>`.
- Чисто виртуален метод `Effect* clone() const` – връща дълбоко копие на ефекта.
- Виртуален деструктор.

`Effect` няма член-данни.

---

**(в) Конкретни ефекти**

Общи правила за всички наследници:
- Конкретната математика на `apply` е по избор – важно е сигналът да се променя смислено според параметъра.
- `clone()` връща указател към конкретния тип (ковариантен тип).
- `describe()` отпечатва името и стойността на параметрите.

**`Overdrive`**
- Член-данни:
  - `float m_drive` – колкото е по-висок, толкова по-изкривен е сигналът. Разумен диапазон: `1.0f`–`5.0f`.
  - `bool m_boosted` – флаг, който показва, че следващото `apply` ще използва удвоено `drive`.
- Конструктор: `Overdrive(float drive)`. Инициализира `m_boosted = false`.
- `apply`: ако `m_boosted == true`, използва `2.0f * m_drive` за единичното извикване и нулира флага; иначе използва `m_drive`. Примерна формула: `output = std::tanh(drive * signal)`.
- `void boost()`: задава `m_boosted = true`. Този метод **не** е в `Effect`.
- `clone()` връща `Overdrive*`. Клонингът копира `m_drive`, но **не** копира `m_boosted` – новият ефект започва "небустнат".

**`Reverb`**
- Член-данни: `float m_room_size` – симулира различни размери зала. Разумен диапазон: `0.0f`–`1.0f`.
- Конструктор: `Reverb(float room_size)`.
- `apply`: смесва входния сигнал със симулирано ехо. Примерна формула: `output = signal * (1.0f - 0.3f * m_room_size) + 0.3f * m_room_size`.
- `clone()` връща `Reverb*`.

**`Delay`**
- Член-данни:
  - `float m_feedback` – каква част от сигнала се "забавя" обратно. Разумен диапазон: `0.0f`–`1.0f`.
  - `float m_last_signal` – запазен сигнал от предишно извикване на `apply` (т.е. `Delay` пази вътрешно състояние между извикванията).
- Конструктор: `Delay(float feedback)`. Инициализира `m_last_signal = 0.0f`.
- `apply`: смесва текущия сигнал с предишния. Примерна формула: `output = signal + m_feedback * m_last_signal; m_last_signal = output;`.
- `clone()` връща `Delay*`. Клонингът копира `m_feedback`, но `m_last_signal` започва от `0.0f` – помислете защо: преходното състояние не принадлежи на "идентичността" на ефекта.

**`Chorus`**
- Член-данни: `float m_depth` – дълбочина на ефекта. Разумен диапазон: `0.0f`–`1.0f`.
- Конструктор: `Chorus(float depth)`.
- `apply`: симулира леко "разтрепване" на сигнала. Примерна формула: `output = signal * (1.0f + m_depth * 0.5f)`.
- `clone()` връща `Chorus*`.

---

**(г) Клас `Pedalboard`**

Дефинирайте клас `Pedalboard`, който управлява веригата от ефекти.

- Член-данни:
  - `Effect* m_effects[8]` – фиксиран масив от указатели към ефекти.
  - `int m_count` – текущ брой добавени ефекти.
  - `ISignalLogger& m_logger` – препратка към логъра. Логърът **не** се притежава от `Pedalboard`.

- Конструктор: `Pedalboard(ISignalLogger& logger)`. Инициализира `m_count = 0` и всички указатели в масива на `nullptr`. *(DIP – педалбордът зависи от абстракцията `ISignalLogger`, не от конкретен логър.)*

- `void add(Effect* effect)` – добавя ефекта на позиция `m_count` и инкрементира `m_count`. При пълен масив изхвърля `std::runtime_error` (опишете го в коментар). С извикването на `add`, `Pedalboard` поема собствеността върху подадения указател.

- `float process(float signal)` – прекарва сигнала последователно през всички ефекти:
  - За всеки ефект `e` в масива: запомня входа в локална променлива, извиква `output = e->apply(signal)`, обновява `signal = output`, и извиква `m_logger.log_stage(e->name(), input, output)`.
  - В края извиква `m_logger.log_chain_result(signal)` и връща крайния сигнал.

- `Pedalboard* clone_chain() const`:
  - Заделя нов `Pedalboard` със **същия** логър (споделя препратката).
  - За всеки ефект в текущия масив извиква полиморфния `clone()` и добавя резултата в новия `Pedalboard` чрез `add`.
  - Връща готовия клонинг. Извикващият е отговорен за изтриването му.

- `void boost_overdrives()`:
  - Обхожда масива.
  - За всеки ефект прави `dynamic_cast<Overdrive*>` и, ако кастът върне ненулев указател, извиква `boost()` върху него.
  - Това е **единственото** място, където `Pedalboard` се нуждае от знание за конкретен наследник на `Effect`. Помислете дали това е добър дизайн – какво биха казали принципите OCP и LSP за него?

- Деструктор: изтрива всички `Effect*` в масива.

- Класът трябва да е **некопируем**: забранете копи-конструктора и копи-присвояването с `= delete`. *(Защо? Защото обектът държи "сурови" указатели и реална собственост върху ефектите – автоматичното копиране би довело до двойно `delete`. Дълбоко клониране се прави експлицитно през `clone_chain`.)*

---

**(д) `main()` – наредете всичко**

В `main`:

1. Създайте `ConsoleSignalLogger console_logger`.
2. Създайте `Pedalboard board(console_logger)`.
3. Добавете в разумен ред: `Overdrive(2.0f)`, `Reverb(0.5f)`, `Delay(0.4f)`, `Chorus(0.3f)`. Всички ефекти заделете с `new`.
4. Обходете веригата и извикайте `describe()` за всеки ефект **през базов указател** `Effect*` – за да се види работещият виртуален диспач.
5. Обработете тестов сигнал `0.8f` чрез `board.process(0.8f)` и отпечатайте резултата.
6. Клонирайте веригата: `Pedalboard* clone = board.clone_chain();`.
7. Извикайте `clone->boost_overdrives()` върху клонинга и обработете същия сигнал `0.8f` отново. Резултатът трябва да се различава от стъпка 5 за този `apply`, в който `Overdrive`-ът е бил бустнат.
8. Демонстрирайте втория логър:
   - Създайте `StatsSignalLogger stats_logger`.
   - Създайте трети `Pedalboard stats_board(stats_logger)` с няколко ефекта по ваш избор.
   - Обработете няколко различни сигнала (напр. `0.2f`, `0.5f`, `0.9f`) – по време на обработката няма да има никакъв изход.
   - В края извикайте `stats_logger.print_report()` и покажете натрупаната статистика.
