# Множествено наследяване

## Съдържание

<!-- генерирайте съдържанието тук -->

---

## Основи на множественото наследяване

C++ позволява един клас да наследява от **повече от един базов клас** едновременно. Синтаксисът е директно разширение на единичното наследяване:

```cpp
class Flyable {
public:
    void fly() const { /* airborne movement */ }
};

class Swimmable {
public:
    void swim() const { /* aquatic movement */ }
};

// Duck inherits from both – it can fly AND swim
class Duck : public Flyable, public Swimmable {
public:
    void quack() const { /* duck-specific */ }
};

Duck d;
d.fly();    // from Flyable
d.swim();   // from Swimmable
d.quack();  // Duck's own
```

Производният клас получава членовете и на двата базови класа. Спецификаторите за достъп (`public`, `protected`, `private`) се задават **поотделно** за всяка база и са независими:

```cpp
class Gadget : public Flyable, private Swimmable {
    // fly() is public (inherited as public)
    // swim() is private (inherited as private – callers of Gadget can't see it)
};
```

---

## Ред на конструиране и разрушаване

Базовите класове се конструират в **реда, в който са изброени** в декларацията за наследяване – отляво надясно. Деструкторите се извикват в **обратен ред**:

```cpp
class A {
public:
    A()  { /* constructed first */ }
    ~A() { /* destroyed last  */ }
};

class B {
public:
    B()  { /* constructed second */ }
    ~B() { /* destroyed first   */ }
};

class C : public A, public B {
public:
    C()  { /* body runs after A and B are constructed */ }
    ~C() { /* body runs before B and A are destroyed  */ }
};

// Construction order:  A → B → C
// Destruction  order:  ~C → ~B → ~A
```

Редът в инициализационния списък на конструктора **няма значение** – компилаторът винаги следва реда на декларацията:

```cpp
class C : public A, public B {
public:
    C() : B(), A() {}   // B() listed first, but A is still constructed first
};
```

> **Правило:** Редът на конструиране се определя от декларацията, не от инициализационния списък. Ако ги напишете в различен ред, много компилатори издават предупреждение.

---

## Конфликт на имена

Ако два базови класа дефинират метод с **едно и също име**, директното извикване е двусмислено и компилаторът отказва да компилира:

```cpp
class Printer {
public:
    void reset() { /* reset printer state */ }
};

class Scanner {
public:
    void reset() { /* reset scanner state */ }
};

class Copier : public Printer, public Scanner {};

Copier c;
c.reset();              // COMPILE ERROR – ambiguous: Printer::reset or Scanner::reset?
c.Printer::reset();     // OK – explicit qualification
c.Scanner::reset();     // OK – explicit qualification
```

Квалификацията с `ClassName::` изрично указва коя версия се има предвид. Алтернативата е да предефинирате метода в производния клас и да решите конфликта там:

```cpp
class Copier : public Printer, public Scanner {
public:
    void reset() {
        Printer::reset();   // call both explicitly
        Scanner::reset();
    }
};

Copier c;
c.reset();   // now unambiguous – calls Copier::reset()
```

Конфликтът се появява дори ако методите имат **различни сигнатури** – самото наличие на едно и също име в два базови класа е достатъчно за неяснота:

```cpp
class A { public: void foo(int) {} };
class B { public: void foo(double) {} };

class C : public A, public B {};

C c;
c.foo(1);       // COMPILE ERROR – ambiguous, despite different signatures
c.A::foo(1);    // OK
```

---

## Диамантен проблем

Класическият проблем при множествено наследяване се появява при следната структура:

```
      GameObject
      /         \
   Actor     Interactable
      \         /
        Merchant
```

```cpp
class GameObject {
public:
    int id;
    void update() { /* common update logic */ }
};

class Actor : public GameObject {
public:
    void move() {}
};

class Interactable : public GameObject {
public:
    void interact() {}
};

class Merchant : public Actor, public Interactable {};
```

`Merchant` наследява `Actor` и `Interactable`, а те двете носят по едно копие на `GameObject`. Резултатът е, че `Merchant` съдържа **две отделни копия** на `GameObject`:

```
Memory layout of Merchant (without virtual inheritance):
  +-------------------------+
  |  Actor::GameObject::id  |   ← GameObject subobject from Actor
  |  (Actor members)        |
  +-------------------------+
  |  Interactable::          |
  |    GameObject::id        |   ← GameObject subobject from Interactable – separate!
  |  (Interactable members) |
  +-------------------------+
  |  (Merchant members)     |
  +-------------------------+
```

Всеки достъп до `GameObject`-членове е двусмислен:

```cpp
Merchant m;
m.id = 42;              // COMPILE ERROR – ambiguous: Actor::GameObject::id or Interactable::GameObject::id?
m.update();             // COMPILE ERROR – ambiguous

m.Actor::id = 42;       // OK – explicit path
m.Interactable::id = 42; // OK – sets the OTHER copy
m.Actor::update();      // OK

// The two GameObject subobjects can have different state:
m.Actor::id = 1;
m.Interactable::id = 99; // m now has contradictory ids – logically wrong
```

Двете копия на `GameObject` са напълно независими и могат да имат различно състояние – което в повечето случаи е логически грешка.

---

## Виртуално наследяване

**Виртуалното наследяване** решава проблема с диаманта: то гарантира, че дори ако един базов клас е достигнат по множество пътища, в крайния обект ще съществува само **едно споделено копие** на него.

Ключовата дума `virtual` се поставя при **непосредствените наследници** на общия базов клас:

```cpp
class GameObject {
public:
    int id;
    void update() {}
};

class Actor        : public virtual GameObject { public: void move()     {} };
class Interactable : public virtual GameObject { public: void interact() {} };

class Merchant : public Actor, public Interactable {};
```

Сега `Merchant` има само **едно** копие на `GameObject`:

```cpp
Merchant m;
m.id = 42;    // OK – unambiguous, single GameObject subobject
m.update();   // OK – unambiguous
```

```
Memory layout of Merchant (with virtual inheritance):
  +------------------------+
  |  Actor vptr            | → points to Actor's vtable (contains offset to GameObject)
  |  (Actor members)       |
  +------------------------+
  |  Interactable vptr     | → points to Interactable's vtable (contains offset to GameObject)
  |  (Interactable members)|
  +------------------------+
  |  (Merchant members)    |
  +------------------------+
  |  GameObject::id        | ← single shared GameObject subobject, at the end
  +------------------------+
```

Виртуалният базов клас е поставен **в края** на обекта и се достига чрез указател/отместване, съхранен във виртуалните таблици. Това добавя малко overhead – допълнителен `vptr` за всеки виртуален базов клас.

> **Правило:** `virtual` при наследяване се поставя там, където проблемът се появява – в **средните** класове (`Actor`, `Interactable`), не в общата база (`GameObject`) и не в крайния наследник (`Merchant`).

---

## Конструктори при виртуално наследяване

Виртуалното наследяване въвежда едно ново правило: **най-производният клас** е отговорен за конструирането на виртуалния базов клас, независимо дали той „вижда" директно тази база.

```cpp
class GameObject {
public:
    int id;
    GameObject(int i) : id(i) {}
};

class Actor : public virtual GameObject {
public:
    Actor() : GameObject(0) {}   // this call is IGNORED when Actor is a virtual base
    Actor(int i) : GameObject(i) {}
};

class Interactable : public virtual GameObject {
public:
    Interactable() : GameObject(0) {}  // also IGNORED when Interactable is a virtual base
};

class Merchant : public Actor, public Interactable {
public:
    // Merchant MUST call GameObject's constructor directly:
    Merchant() : GameObject(1), Actor(), Interactable() {}
    //           ^^^^^^^^^^^^^^
    //           If omitted, GameObject's default constructor is called (if it exists).
    //           Actor() and Interactable() each "want" to construct GameObject, but their
    //           GameObject(...) calls in their initializer lists are silently skipped.
};
```

Когато `Merchant` е конкретният тип, извикванията `GameObject(0)` в `Actor()` и `Interactable()` биват **игнорирани**. Само извикването в `Merchant` има ефект. Ако `Merchant` пропусне извикването, се търси конструктор по подразбиране на `GameObject` – при липса на такъв се получава грешка при компилация.

Редът на конструиране при виртуално наследяване е:

1. Виртуалните базови класове – по реда на тяхното срещане в дълбочина (depth-first, left-to-right).
2. Невиртуалните базови класове – отляво надясно.
3. Тялото на конструктора на производния клас.

```cpp
// For Merchant : Actor, Interactable, both virtual from GameObject:
// 1. GameObject(1)    ← virtual base, called by Merchant directly
// 2. Actor()          ← left base (skips its GameObject(...) initializer)
// 3. Interactable()   ← right base (skips its GameObject(...) initializer)
// 4. Merchant()       ← body
```

> **Правило:** Когато пишете клас, който използва виртуално наследяване, винаги мислете: „Кой е отговорен за конструирането на виртуалната база?" Отговорът е винаги най-производният клас в дадена верига на конструиране.

---

## Корекция на указателя при casting

При множествено наследяване адресите на различните базови подобекти в един и същ обект **не са непременно равни**. Upcast към различни базови класове може да промени числовата стойност на указателя:

```cpp
class A { int a; };
class B { int b; };
class C : public A, public B {};

C obj;
C* pc = &obj;
A* pa = pc;   // may equal pc (A is first base)
B* pb = pc;   // almost certainly NOT equal to pc – adjusted to point at B subobject

// Comparing as void* shows the adjustment:
// (void*)pa == (void*)pc   → likely true
// (void*)pb == (void*)pc   → likely false – pb points further into the object
```

`static_cast` извършва корекцията правилно **само ако типовете са известни**. `reinterpret_cast` никога не коригира – използването му за downcast при множествено наследяване е неопределено поведение:

```cpp
B* pb = static_cast<B*>(pc);      // correct – adjusts pointer
B* pb2 = reinterpret_cast<B*>(pc); // WRONG – no adjustment, UB when dereferenced
```

`dynamic_cast` извършва корекцията правилно и при кръстосан cast (crosscast) между несвързани базови класове в рамките на един и същ обект:

```cpp
A* pa = new C();
B* pb = dynamic_cast<B*>(pa);   // crosscast: A* → B* through the shared C object – works!
B* pb2 = static_cast<B*>(pa);   // COMPILE ERROR – A and B are unrelated by inheritance
```

---

## Правилото за доминиране

При виртуално наследяване, ако виртуалният базов клас декларира виртуален метод и **само един** от средните класове го предефинира, предефиниращата версия доминира над базовата за целия диамант – дори за пътищата, които минават през класа, който **не** е предефинирал:

```cpp
class GameObject {
public:
    void update(float delta_time) { /* common update logic */ };
    void update(const int delta_time, const int other_param) {} // overload, not override
};

class Actor : public virtual GameObject {
public:
    void update(float delta_time) override { /* move, animate */ }
};

class Interactable : public virtual GameObject {
    // does NOT override update()
};

class Merchant : public Actor, public Interactable {
    void sell() {
        update(2, 3);   // calls Actor::update(float) – the overload is hidden by Actor's override, even though Interactable doesn't override at all
    }
};
```

Правилото за доминиране се прилага, защото е ясно кой клас е „по-специфичен" в йерархията. Ако **и двата** средни класа предефинират метода, отново се получава двусмислие:

```cpp
class Interactable : public virtual GameObject {
public:
    void update() override { /* highlight, pulse */ }   // now Interactable also overrides
};

Merchant m;
m.update();              // COMPILE ERROR – Actor::update and Interactable::update are equally specific
m.Actor::update();       // OK – explicit
```

---

## Смесване на виртуално и невиртуално наследяване

Ако един клас наследява един и същ базов клас едновременно виртуално и невиртуално (по различни пътища), се получава **едно споделено** виртуално копие и **едно или повече отделни** невиртуални копия:

```cpp
class Base { public: int x; };

class V : public virtual Base {};   // virtual path
class N : public Base {};           // non-virtual path

class Mixed : public V, public N {};

Mixed m;
m.V::x = 1;   // accesses the single virtual Base subobject
m.N::x = 2;   // accesses the separate non-virtual Base subobject
// m.x – ambiguous!
```

Резултатът е обект с **две** копия на `Base` – едно споделено (виртуалното) и едно отделно (невиртуалното). Това почти никога не е желано. Избягвайте смесването.

---

## Добра употреба – интерфейси и миксини (mixins)

Множественото наследяване е най-добре мотивирано когато **повечето или всички** базови класове са чисто абстрактни (интерфейси без данни). В такива случаи не може да съществува диамант с данни и проблемите с двусмислие са редки.

**Интерфейсен модел:**

```cpp
class ISerializable {
public:
    virtual void serialize(const char* path) const = 0;
    virtual void deserialize(const char* path) = 0;
    virtual ~ISerializable() = default;
};

class IDrawable {
public:
    virtual void draw() const = 0;
    virtual ~IDrawable() = default;
};

class IClickable {
public:
    virtual void on_click() = 0;
    virtual ~IClickable() = default;
};

// A button is drawable, clickable and can be saved to disk
class Button : public IDrawable, public IClickable, public ISerializable {
public:
    void draw()                         const override { /* render button */ }
    void on_click()                           override { /* handle click */ }
    void serialize(const char* path)    const override { /* write to file */ }
    void deserialize(const char* path)        override { /* read from file */ }
};
```

Тъй като интерфейсите нямат данни и почти нямат имплементация, конфликти на имена се появяват само ако два интерфейса декларират метод с **едно и също** служебно значение – в такъв случай едната имплементация обикновено удовлетворява и двата.

**Миксин модел** – клас, добавящ готова функционалност без да налага конкретна йерархия:

```cpp
// Mixin: adds reference-counting behaviour to any class
class RefCounted {
    int ref_count = 0;
public:
    void add_ref()    { ++ref_count; }
    void release()    { if (--ref_count == 0) delete this; }
    int  use_count()  const { return ref_count; }
protected:
    virtual ~RefCounted() = default;   // virtual: deletion through this base must work
};

class Texture : public IDrawable, public RefCounted {
public:
    void draw() const override { /* render texture */ }
};

Texture* t = new Texture();
t->add_ref();
// ... share t ...
t->release();   // deletes when ref_count reaches 0
```

> **Правило:** Използвайте множествено наследяване предимно за комбиниране на **интерфейси** (чисто абстрактни класове без данни). Когато и двата базови класа имат данни или имплементация, вместо множествено наследяване помислете за композиция.

---

## Допълнителни ресурси

- [Добро обяснение на виртуални таблици в контекста на множествено наследяване](https://shaharmike.com/cpp/vtable-part2/)

---

## Задачи

**Задача 1 – Конфликт на имена**

Дефинирайте два класа `Logger` и `Formatter`, всеки с метод `reset()`. Направете клас `ReportEngine`, наследяващ и двата. Покажете как се получава грешка при компилация при директно извикване на `reset()` и я разрешете по два различни начина: с явна квалификация от страна на извикващия и с предефиниране в `ReportEngine`.

---

**Задача 2 – Диамантът без виртуално наследяване**

Изградете диамантена йерархия `Vehicle → MotorVehicle, FloatingVehicle → Hovercraft`. `Vehicle` съдържа поле `speed`. Без виртуално наследяване покажете: (а) колко копия на `speed` има в `Hovercraft`; (б) как се присвояват различни стойности на двете копия; (в) защо това е логически проблем.

---

**Задача 3 – Виртуално наследяване**

Повторете Задача 2 с `virtual` при `MotorVehicle` и `FloatingVehicle`. Покажете, че вече има само едно `speed`. Напишете конструктор на `Hovercraft`, който правилно инициализира `Vehicle` директно – дори когато `MotorVehicle` и `FloatingVehicle` също имат конструктори с параметри.

---

**Задача 4 – Интерфейсна йерархия**

Дефинирайте интерфейсите `IReadable` (метод `read()`), `IWritable` (метод `write()`), `ISeekable` (метод `seek(int pos)`). Имплементирайте `FileStream : IReadable, IWritable, ISeekable` и `NetworkStream : IReadable, IWritable` (мрежата няма `seek`). Напишете функция, която приема `IReadable&` и чете данни – тя трябва да работи и с двата потока. Напишете друга, която приема `ISeekable*` и прескача позиция само ако обектът поддържа `seek` (използвайте `dynamic_cast`).

---

**Задача 5 – Корекция на указателя**

Дефинирайте `class A { int a; };`, `class B { int b; };`, `class C : public A, public B {};`. В `main` създайте `C` обект, вземете `C*`, `A*` и `B*` към него. Отпечатайте числовите стойности на трите указателя (чрез `reinterpret_cast<uintptr_t>`). Обяснете защо `B*` се различава. Опитайте `dynamic_cast<B*>` от `A*` и обяснете кога успява и кога не.
