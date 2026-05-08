# Наследяване – идея и основни концепции

## Съдържание

- [Наследяване – идея и основни концепции](#наследяване--идея-и-основни-концепции)
  - [Съдържание](#съдържание)
  - [Идея и мотивация](#идея-и-мотивация)
    - [Проблемът без наследяване](#проблемът-без-наследяване)
    - [Решението с наследяване](#решението-с-наследяване)
    - [Терминология](#терминология)
  - [Синтаксис на наследяването](#синтаксис-на-наследяването)
    - [Какво наследява производният клас](#какво-наследява-производният-клас)
  - [Спецификатори за достъп при наследяване](#спецификатори-за-достъп-при-наследяване)
    - [`public` наследяване (най-честото)](#public-наследяване-най-честото)
    - [`protected` наследяване](#protected-наследяване)
    - [`private` наследяване (изразява "реализирано чрез")](#private-наследяване-изразява-реализирано-чрез)
    - [Обобщение](#обобщение)
  - [Конструктори и деструктори при наследяване](#конструктори-и-деструктори-при-наследяване)
    - [Ред на конструиране](#ред-на-конструиране)
    - [Предаване на аргументи към базовия конструктор](#предаване-на-аргументи-към-базовия-конструктор)
    - [Ред на разрушаване](#ред-на-разрушаване)
    - [Ако базовият клас няма конструктор по подразбиране](#ако-базовият-клас-няма-конструктор-по-подразбиране)
  - [Достъп до членове на базовия клас](#достъп-до-членове-на-базовия-клас)
    - [Оператор за разрешаване на обхват при достъп до базов клас](#оператор-за-разрешаване-на-обхват-при-достъп-до-базов-клас)
    - [`protected` – достъп само в йерархията](#protected--достъп-само-в-йерархията)
  - [Скриване на имена](#скриване-на-имена)
    - [Скриване на член-данни](#скриване-на-член-данни)
    - [Задача](#задача)
  - [Преобразуване между базов и производен клас](#преобразуване-между-базов-и-производен-клас)
    - [Нагоре по йерархията (upcasting)](#нагоре-по-йерархията-upcasting)
    - [Надолу по йерархията (downcasting)](#надолу-по-йерархията-downcasting)
    - [Масиви и слайсинг](#масиви-и-слайсинг)
  - [Задачи](#задачи)

---

## Идея и мотивация

**Наследяването** е механизъм, чрез който един клас (`производен клас`, derived class) придобива членовете на друг клас (`базов клас`, base class) и може да ги разширява или специализира. То изразява отношението **"е вид"** (is-a) между типове.

### Проблемът без наследяване

Представете си, че моделирате служители в компания. Без наследяване повтаряте едни и същи полета навсякъде:

```cpp
class Employee {
    char name[64];
    int  id;
    double salary;
public:
    // ... constructor, getters, etc.
    void print() const { /* print name, id, salary */ }
};

class Manager {
    char   name[64];     // duplicated
    int    id;           // duplicated
    double salary;       // duplicated
    int    team_size;    // Manager-specific
public:
    // ... all the same methods, plus team_size logic
    void print() const { /* same print + team_size */ }
};

class Intern {
    char   name[64];     // duplicated again
    int    id;           // duplicated again
    double salary;       // duplicated again
    int    duration_months; // Intern-specific
public:
    // ...
};
```

Кодът е повторен три пъти. Ако трябва да добавите ново поле към всички служители (например `department`), трябва да го добавите и в трите класа поотделно.

### Решението с наследяване

```cpp
class Employee {
    char   name[64];
    int    id;
    double salary;
public:
    Employee(const char* name, int id, double salary);
    void print() const;
};

// Manager "is an" Employee, and additionally has a team
class Manager : public Employee {
    int team_size;
public:
    Manager(const char* name, int id, double salary, int team_size);
    void print_team() const;
};

// Intern "is an" Employee, and additionally has a contract duration
class Intern : public Employee {
    int duration_months;
public:
    Intern(const char* name, int id, double salary, int months);
};
```

`Manager` и `Intern` автоматично получават всички членове на `Employee`. Добавяте `department` само в `Employee` – и двата производни класа го придобиват веднага.

### Терминология

```
Employee          ← базов клас (base class), суперклас, родителски клас
   ↑
Manager           ← производен клас (derived class), субклас, подклас, дъщерен клас
```

Производният клас **разширява** базовия – той съдържа всичко от базовия клас плюс собствените си добавки.

---

## Синтаксис на наследяването

```cpp
class Base {
public:
    int pub;
protected:
    int prot;
private:
    int priv;   // derived class cannot access this directly
};

// Syntax: class Derived : access_specifier Base
class Derived : public Base {
public:
    void show() {
        pub  = 1;   // OK – public in Base, stays public
        prot = 2;   // OK – protected in Base, accessible in derived
        // priv = 3; // ERROR – private in Base, invisible everywhere outside Base
    }
};
```

### Какво наследява производният клас

Производният клас наследява **всички членове** на базовия, включително:

```cpp
class Animal {
    char  name[32];
    float weight;
public:
    Animal(const char* n, float w);
    void  breathe() const;
    float get_weight() const { return weight; }
};

class Dog : public Animal {
    char breed[32];
public:
    Dog(const char* name, float weight, const char* breed);
    void bark() const;
};

Dog d("Rex", 30.0f, "Labrador");
d.breathe();       // inherited from Animal
d.get_weight();    // inherited from Animal
d.bark();          // Dog's own method
```

Какво **не** се наследява:

```cpp
// Constructors: NOT inherited (but can be called via initializer list)
// Destructors:  NOT inherited (but base destructor is called automatically)
// operator=:    NOT inherited (each class manages its own assignment)
// Friend declarations: NOT inherited (friendship is not transitive)
```

---

## Спецификатори за достъп при наследяване

Начинът на наследяване (`public`, `protected`, `private`) определя как членовете на базовия клас се виждат **отвън** производния клас. Той **не** променя видимостта вътре в производния клас.

### `public` наследяване (най-честото)

Запазва нивото на достъп от базовия клас. Изразява отношението "е вид" (is-a):

```cpp
class Base {
public:    int pub;
protected: int prot;
private:   int priv;
};

class PublicDerived : public Base {
    // pub  remains public    → visible to everyone
    // prot remains protected → visible only inside PublicDerived and its descendants
    // priv remains invisible
};

PublicDerived obj;
obj.pub = 1;    // OK – public
// obj.prot = 2; // ERROR – protected
```

### `protected` наследяване

Всичко `public` от базовия клас става `protected` в производния – видимо само вътре в йерархията, не отвън:

```cpp
class ProtectedDerived : protected Base {
    // pub  becomes protected → visible inside ProtectedDerived and its descendants
    // prot remains protected
    // priv remains invisible
};

ProtectedDerived obj;
// obj.pub = 1;  // ERROR – pub is now protected
```

### `private` наследяване (изразява "реализирано чрез")

Всичко от базовия клас става `private` в производния – дори наследниците на производния клас нямат достъп:

```cpp
class PrivateDerived : private Base {
    // pub  becomes private → visible only inside PrivateDerived
    // prot becomes private → visible only inside PrivateDerived
    // priv remains invisible
};

class FurtherDerived : public PrivateDerived {
    void show() {
        // pub = 1;  // ERROR – pub is private in PrivateDerived
    }
};
```

### Обобщение

| Член в Base | `public` наследяване | `protected` наследяване | `private` наследяване |
|:---|:---:|:---:|:---:|
| `public` | `public` | `protected` | `private` |
| `protected` | `protected` | `protected` | `private` |
| `private` | недостъпен | недостъпен | недостъпен |

> **На практика:** Почти винаги ще използвате `public` наследяване. `private` наследяването е рядко – обикновено в ситуации, където композицията (член-обект) е по-подходяща, но интерфейсът на базовия клас е нужен вътрешно.

---

## Конструктори и деструктори при наследяване

### Ред на конструиране

Когато се създава обект от производен клас, конструкторите се извикват в следния ред:

1. Конструкторът на **базовия клас**.
2. Конструкторите на **член-данните** на производния клас (по реда на декларация).
3. Тялото на **конструктора на производния клас**.

```cpp
class Base {
public:
    Base()          { /* 1. called first */ }
    Base(int value) { /* 1. called first (parameterized) */ }
};

class Derived : public Base {
    int extra;
public:
    Derived() : Base(), extra(0) {
        // 3. called last – Base() and extra have been initialized
    }
};
```

### Предаване на аргументи към базовия конструктор

Базовият конструктор се извиква **в списъка с инициализатори** на производния. Не може да се извика в тялото:

```cpp
class Shape {
    char label[32];
    int  color;
public:
    Shape(const char* lbl, int col);   // must be called from Derived's init list
};

class Circle : public Shape {
    float radius;
public:
    // Passing arguments up to Shape's constructor
    Circle(const char* label, int color, float r)
        : Shape(label, color)   // base constructor called here
        , radius(r)             // then own members initialized
    {}
};

class Rectangle : public Shape {
    float width, height;
public:
    Rectangle(const char* label, int color, float w, float h)
        : Shape(label, color)
        , width(w)
        , height(h)
    {}
};
```

### Ред на разрушаване

Деструкторите се извикват в **обратен ред** спрямо конструирането:

1. Тялото на **деструктора на производния клас**.
2. Деструкторите на **член-данните** на производния клас (в обратен ред).
3. Деструкторът на **базовия клас**.

```cpp
class Base {
public:
    ~Base() { /* 3. called last */ }
};

class Derived : public Base {
    Resource* res;
public:
    Derived() : res(new Resource()) {}
    ~Derived() {
        delete res;   // 1. free own resources first
        // 2. member destructors run here (automatic)
        // 3. then ~Base() runs automatically
    }
};
```

> **Правило:** Производният клас трябва да освободи **само своите** ресурси. Ресурсите на базовия клас се освобождават от неговия деструктор автоматично.

### Ако базовият клас няма конструктор по подразбиране

Ако базовият клас не може да се конструира без аргументи, производният клас **задължително** трябва да го извика явно в списъка с инициализатори:

```cpp
class Named {
    char name[64];
public:
    // No default constructor – name is always required
    Named(const char* n);
};

class NamedCounter : public Named {
    int count;
public:
    // Must explicitly call Named(n) – there is no Named() to fall back to
    NamedCounter(const char* n, int start)
        : Named(n)       // required
        , count(start)
    {}

    // NamedCounter() {}  // would be a compile error – Named() does not exist
};
```

---

## Достъп до членове на базовия клас

Производният клас достъпва наследените `public` и `protected` членове директно, без специален синтаксис:

```cpp
class Vehicle {
protected:
    int speed;
    char brand[32];
public:
    Vehicle(int s, const char* b) : speed(s) {
        // copy brand
    }
    int get_speed() const { return speed; }
};

class Car : public Vehicle {
    int doors;
public:
    Car(int s, const char* b, int d) : Vehicle(s, b), doors(d) {}

    void accelerate(int delta) {
        speed += delta;   // direct access to protected member
    }

    void print() const {
        // get_speed() is public in Vehicle – callable directly
        // speed is protected – accessible directly inside Car
    }
};
```

### Оператор за разрешаване на обхват при достъп до базов клас

Когато производният клас скрие член на базовия (виж следващия раздел), можете да достъпите версията на базовия клас явно чрез `Base::member`:

```cpp
class Printer {
public:
    void print() const {
        // base version
    }
};

class FancyPrinter : public Printer {
public:
    void print() const {
        Printer::print();   // explicitly call base class version
        // ... additional fancy output
    }
};
```

### `protected` – достъп само в йерархията

`protected` е специален: достъпен е в производните класове, но **не** е достъпен отвън:

```cpp
class BankAccount {
protected:
    double balance;   // subclasses can read and modify this
public:
    double get_balance() const { return balance; }
};

class SavingsAccount : public BankAccount {
    double interest_rate;
public:
    void apply_interest() {
        balance *= (1.0 + interest_rate);   // OK – protected in base
    }
};

BankAccount acc;
// acc.balance = 1000;   // ERROR – protected, not accessible from outside
acc.get_balance();        // OK – public accessor
```

---

## Скриване на имена

Когато производният клас декларира член **със същото име** като член на базовия, производният **скрива** базовия (name hiding). Това **не е** предефиниране (override) – просто базовата версия вече не се вижда директно.

```cpp
class Base {
public:
    void show() const {
        // base version
    }

    void show(int x) const {
        // overloaded base version
    }
};

class Derived : public Base {
public:
    // This declaration hides ALL overloads of Base::show, not just the matching one
    void show() const {
        // derived version
    }
};

Derived d;
d.show();      // calls Derived::show()
// d.show(5);  // ERROR – Base::show(int) is hidden!

// To call the base version explicitly:
d.Base::show();      // OK
d.Base::show(5);     // OK
```

> **Важно:** Скриването на имена важи за **всички** претоварени версии в базовия клас, не само за тази с еднаквия подпис. Ако искате да запазите базовите претоварвания и само да добавите нови, използвайте `using`:

```cpp
class Derived : public Base {
public:
    using Base::show;   // bring all Base::show overloads into scope

    void show() const {
        // adds a new overload, does not hide the others
    }
};

Derived d;
d.show();      // calls Derived::show()
d.show(5);     // OK – Base::show(int) is visible via using
```

### Скриване на член-данни

Скриването важи и за полета, но е почти винаги грешка:

```cpp
class Base {
public:
    int value = 10;
};

class Derived : public Base {
public:
    int value = 20;   // hides Base::value – now there are TWO fields named value
};

Derived d;
d.value;          // Derived::value (20)
d.Base::value;    // Base::value (10) – still exists, just hidden
```

> **Правило:** Никога не скривайте полета. Ако производният клас трябва да промени поведение спрямо базовото поле, използвайте методи-аксесори.

### Задача

Дефинирайте клас `BasicLogger` с метод `void log(const char* msg) const` (отпечатва съобщението).

После дефинирайте `TimestampLogger : public BasicLogger` с:
- Метод `void log(const char* msg) const`, който **скрива** базовата версия и добавя префикс `[HH:MM]` преди съобщението (използвайте фиктивен timestamp).
- `using BasicLogger::log` **не** трябва да присъства – целта е да демонстрирате скриването.
- Явно извикайте `BasicLogger::log(msg)` от тялото на `TimestampLogger::log`, за да не дублирате логиката за извеждане.

---

## Преобразуване между базов и производен клас

### Нагоре по йерархията (upcasting)

Обект от производен клас може да се третира като обект от базовия клас. Преобразуването е **имплицитно** и **безопасно**:

```cpp
class Animal {
public:
    void breathe() const { /* ... */ }
};

class Dog : public Animal {
public:
    void bark() const { /* ... */ }
};

Dog dog;

Animal& ref = dog;      // OK – upcasting by reference (no copy)
Animal* ptr = &dog;     // OK – upcasting by pointer (no copy)

ref.breathe();   // OK – Animal interface is available
// ref.bark();   // ERROR – Animal does not have bark()
```

При преобразуване **по стойност** се получава **рязане** (slicing) – производните членове се губят:

```cpp
Animal a = dog;   // SLICING: only the Animal part is copied, Dog-specific data lost
// a now behaves entirely as an Animal, dog-specific fields are gone
```

> **Правило:** Предавайте полиморфни обекти **по указател или референция**, никога по стойност. Слайсингът е тиха грешка – компилаторът не предупреждава.

### Надолу по йерархията (downcasting)

Обратното преобразуване – от базов към производен – е **не** имплицитно и е **опасно** без допълнителни проверки:

```cpp
Animal* ptr = new Dog();   // OK – upcast

// static_cast: no runtime check – undefined behaviour if ptr doesn't actually point to Dog
Dog* d = static_cast<Dog*>(ptr);   // compiles, but dangerous if ptr is not a Dog
d->bark();

// Without virtual functions there is no safe way to verify the actual type.
// Downcasting is covered in depth when virtual functions and dynamic_cast are introduced.
```

> **На този етап:** Избягвайте downcast. Ако ви се налага, това е сигнал, че дизайнът на йерархията трябва да се преосмисли.

### Масиви и слайсинг

Слайсингът е особено коварен при масиви:

```cpp
Dog dogs[3];
Animal* arr = dogs;   // appears to work – array decays to pointer

arr[1].breathe();     // UNDEFINED BEHAVIOUR if sizeof(Dog) != sizeof(Animal)
                      // pointer arithmetic uses sizeof(Animal), not sizeof(Dog)
```

> **Правило:** Никога не съхранявайте производни обекти в масив от базов тип. Използвайте масив от **указатели**.

---

## Задачи

**Задача 1 – Йерархия на превозни средства**

Дефинирайте клас `Vehicle` с полета `brand` (char[32]) и `year` (int) и метод `void print() const`, който ги отпечатва. После дефинирайте:
- `Car : public Vehicle` – добавя поле `doors` (int).
- `Truck : public Vehicle` – добавя поле `payload_tons` (double).

Всеки производен клас трябва да има конструктор, който приема всички нужни аргументи и ги предава нагоре по веригата чрез initializer list. Всеки клас трябва да има собствен `print()`, който извиква `Vehicle::print()` и добавя специфичното поле.

Създайте по един обект от всеки тип в `main()` и извикайте `print()`.

---

**Задача 2 – Банкова сметка**

Дефинирайте клас `BankAccount` с `protected` поле `balance` (double) и публични методи `deposit(double)` и `get_balance() const`. Дефинирайте `SavingsAccount : public BankAccount` с допълнително поле `interest_rate` (double) и метод `apply_interest()`, който директно модифицира `balance` (достъпен като `protected`).

Демонстрирайте, че `balance` е недостъпен отвън (`account.balance = ...` трябва да е грешка), но е достъпен вътре в `SavingsAccount::apply_interest()`.

---

**Задача 3**

Дефинирайте клас `Sensor` с метод `void read() const` (не виртуален), който отпечатва `"Sensor::read"`. Дефинирайте `TemperatureSensor : public Sensor` с метод `void read() const`, който отпечатва `"TemperatureSensor::read"`.

Напишете следния код и преди да го пуснете, запишете кое от двете се очаква:

```cpp
TemperatureSensor ts;
Sensor* ptr = &ts;
ptr->read();   // Which version is called?
ts.read();     // Which version is called?
```

Пуснете програмата и обяснете резултата. После добавете `virtual` към `Sensor::read` и повторете.

---

**Задача 4 – Комбинирана: зоологическа градина**

Дефинирайте базов клас `Animal` с полета `name` (char[32]) и `age` (int), конструктор и метод `void describe() const`.

Дефинирайте три производни класа:
- `Mammal : public Animal` – добавя `fur_color` (char[32]).
- `Bird : public Animal` – добавя `wingspan_cm` (double).
- `Reptile : public Animal` – добавя `is_venomous` (bool).

Всеки клас трябва да има конструктор с пълни аргументи и `describe()`, който разширява базовия.

В `main()` създайте масив от **указатели** към `Animal`:

```cpp
Animal* zoo[5] = {
    new Mammal("Lion", 6, "golden"),
    new Bird("Eagle", 3, 210.0),
    new Reptile("Cobra", 4, true),
    new Mammal("Wolf", 5, "grey"),
    new Bird("Parrot", 2, 45.0)
};
```

Обходете масива и извикайте `describe()` за всеки елемент.

