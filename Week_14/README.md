# Шаблони на класове (class templates)

## Съдържание

- [Шаблони на класове (class templates)](#шаблони-на-класове-class-templates)
  - [Съдържание](#съдържание)
  - [Преговор: шаблони на функции](#преговор-шаблони-на-функции)
  - [Идея на шаблоните на класове](#идея-на-шаблоните-на-класове)
  - [Синтаксис и инстанциране](#синтаксис-и-инстанциране)
  - [Дефиниране на методи извън тялото на класа](#дефиниране-на-методи-извън-тялото-на-класа)
  - [Дедукция на шаблонни аргументи (CTAD)](#дедукция-на-шаблонни-аргументи-ctad)
  - [Не-типови шаблонни параметри](#не-типови-шаблонни-параметри)
  - [Шаблонни параметри по подразбиране](#шаблонни-параметри-по-подразбиране)
  - [Шаблонни методи в шаблонен клас (member templates)](#шаблонни-методи-в-шаблонен-клас-member-templates)
  - [Пълна специализация](#пълна-специализация)
  - [Частична специализация](#частична-специализация)
  - [Зависими имена: `typename` и `template`](#зависими-имена-typename-и-template)
  - [Статични член-данни в шаблони](#статични-член-данни-в-шаблони)
  - [Приятелски функции в шаблонни класове](#приятелски-функции-в-шаблонни-класове)
  - [Разделна компилация и шаблони](#разделна-компилация-и-шаблони)
  - [Задачи](#задачи)

---

## Преговор: шаблони на функции

В [седмица 5](../Week_05) видяхме, че **шаблоните на функции** ни позволяват да напишем един общ алгоритъм, който компилаторът инстанцира за конкретен тип:

```cpp
template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}

maxValue(3, 5);      // instantiates maxValue<int>
maxValue(2.5, 1.0);  // instantiates maxValue<double>
```

Шаблонът е **рецепта**, а не код. Конкретната функция се генерира по време на **компилация**, когато бъде извикана с реален тип.

Същата идея се пренася от функции върху **класове**. Вместо да параметризираме един алгоритъм, параметризираме цяла структура от данни заедно с всичките ѝ методи.

---

## Идея на шаблоните на класове

Често една и съща структура от данни има смисъл за много различни типове. „Двойка от две стойности", „динамичен масив", „стек" — логиката е еднаква, различава се само типът на съхраняваните елементи.

Без шаблони бихме дублирали кода:

```cpp
struct IntPair   { int first;    int second;    };
struct DoublePair{ double first; double second; };
// ... и така за всеки нов тип
```

**Шаблонът на клас** ни позволява да напишем структурата веднъж и да я инстанцираме за произволен тип:

```cpp
template <typename T>
struct Pair {
    T first;
    T second;
};

Pair<int>         p1;   // first and second are int
Pair<double>      p2;   // first and second are double
Pair<std::string> p3;
```

> **Правило:** Шаблонът на клас не е тип. `Pair` сам по себе си не е тип — `Pair<int>` е тип. Едва при подаване на конкретен аргумент компилаторът създава реален клас (**инстанция**).

---

## Синтаксис и инстанциране

По-пълен пример — шаблонен стек с фиксиран капацитет:

```cpp
template <typename T>
class Stack {
    static const size_t CAPACITY = 100;
    T      data[CAPACITY];
    size_t top_idx = 0;

public:
    void push(const T& value) {
        if (top_idx < CAPACITY)
            data[top_idx++] = value;
    }

    void pop() {
        if (top_idx > 0)
            --top_idx;
    }

    const T& top() const { return data[top_idx - 1]; }
    bool     empty() const { return top_idx == 0; }
    size_t   size()  const { return top_idx; }
};
```

Параметърът `T` може да се използва навсякъде в тялото на класа — като тип на член-данни, на параметри и на връщани стойности.

**Инстанциране** — подаваме конкретен тип в ъглови скоби:

```cpp
Stack<int>         intStack;
Stack<std::string> wordStack;

intStack.push(1);
intStack.push(2);
wordStack.push("hello");
```

Компилаторът генерира **отделен клас** за всяка различна инстанция. `Stack<int>` и `Stack<std::string>` са напълно различни типове — не са свързани чрез наследяване и не са взаимозаменяеми.

Важна особеност: член-функциите се инстанцират **лениво** — компилаторът генерира кода на даден метод само ако той реално бъде извикан. Това позволява да инстанцираме `Stack<T>` с тип, който не поддържа всички операции, стига да не извикваме методите, изискващи ги.

---

## Дефиниране на методи извън тялото на класа

Когато искаме да разделим декларацията от дефиницията, всяка дефиниция трябва да повтори `template`-главата и да квалифицира името с `ИмеНаКласа<T>::`:

```cpp
template <typename T>
class Stack {
    static const size_t CAPACITY = 100;
    T      data[CAPACITY];
    size_t top_idx = 0;

public:
    void push(const T& value);
    const T& top() const;
    bool empty() const;
};

template <typename T>
void Stack<T>::push(const T& value) {
    if (top_idx < CAPACITY)
        data[top_idx++] = value;
}

template <typename T>
const T& Stack<T>::top() const {
    return data[top_idx - 1];
}

template <typename T>
bool Stack<T>::empty() const {
    return top_idx == 0;
}
```

Забележете:

- Преди всяка дефиниция стои `template <typename T>`.
- Квалификаторът е `Stack<T>::`, **не** само `Stack::` — отнасяме се към инстанцията.

> **Правило:** Извън тялото на класа квалификаторът е `ИмеНаКласа<Параметри>::`. Самото `Stack::` без аргументи е валидно само **вътре** в обхвата на шаблона (т.нар. injected-class-name).

---

## Дедукция на шаблонни аргументи (CTAD)

До C++17 при **класовете** типовете трябваше да се задават явно — компилаторът не можеше да ги изведе от конструктора:

```cpp
Pair<int> p{1, 2};   // before C++17 — mandatory
```

От **C++17** работи **CTAD** (Class Template Argument Deduction) — компилаторът извежда аргументите от конструктора:

```cpp
Pair p{1, 2};           // C++17: deduces Pair<int>
std::vector v{1, 2, 3}; // deduces std::vector<int>
std::pair pr{1, 2.5};   // deduces std::pair<int, double>
```

За по-сложни случаи можем да дадем **deduction guide** — явна подсказка на компилатора:

```cpp
template <typename T>
struct Pair {
    T first;
    T second;
    Pair(T a, T b) : first(a), second(b) {}
};

// deduction guide: two T arguments → Pair<T>
template <typename T>
Pair(T, T) -> Pair<T>;

Pair p{3, 4};   // OK — Pair<int>
```

> **Правило:** CTAD работи само при инициализация на обект, не при деклариране на указатели или членове. `Pair* ptr;` изисква явен аргумент.

---

## Не-типови шаблонни параметри

Шаблонен параметър може да бъде и **константна стойност**, известна по време на компилация — най-често цяло число. Това е особено полезно за структури с фиксиран размер:

```cpp
template <typename T, size_t N>
class Array {
    T data[N];

public:
    size_t size() const { return N; }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }
};

Array<int, 5>    a;   // array of 5 ints
Array<double, 100> b; // array of 100 doubles
```

`N` е **част от типа**: `Array<int, 5>` и `Array<int, 10>` са различни типове. Размерът е известен при компилация, затова `data` е истински масив без динамично заделяне — точно така работи `std::array<T, N>`.

Не-типовите параметри трябва да са константни изрази:

```cpp
Array<int, 5> ok;        // OK — literal

const size_t k = 8;
Array<int, k> ok2;       // OK — k is constexpr

size_t n;
std::cin >> n;
Array<int, n> bad;       // ERROR — n is not known at compile time
```

> **Правило:** Не-типовите параметри стават **част от типа**. Различни стойности произвеждат различни, несъвместими инстанции.

---

## Шаблонни параметри по подразбиране

Шаблонните параметри могат да имат стойности по подразбиране — точно като аргументите на функции:

```cpp
template <typename T, size_t N = 16>
class Buffer {
    T data[N];
    // ...
};

Buffer<int>      b1;   // N = 16 — default
Buffer<int, 32>  b2;   // N = 32 — explicit
```

Параметрите по подразбиране се четат отляво надясно — щом един параметър има стойност по подразбиране, всички следващи също трябва да имат:

```cpp
// second parameter has a default → OK
template <typename T, typename Alloc = std::allocator<T>>
class vector { /* ... */ };

// first without default, second with → OK
// first with default, second without → COMPILE ERROR
```

Така работи например `std::vector` — вторият параметър (алокаторът) има стойност по подразбиране, затова обикновено пишем само `std::vector<int>`.

---

## Шаблонни методи в шаблонен клас (member templates)

Метод на шаблонен клас може да въведе **собствени** шаблонни параметри, независими от тези на класа. Класически пример — конструктор за преобразуване:

```cpp
template <typename T>
struct Pair {
    T first;
    T second;

    Pair(T a, T b) : first(a), second(b) {}

    // member template: construct Pair<T> from a Pair<U> of a different type
    template <typename U>
    Pair(const Pair<U>& other)
        : first(other.first), second(other.second) {}
};

Pair<double> pd{1.5, 2.5};
Pair<int>    pi = pd;   // OK — U = double, values converted to int
```

`template <typename U>` е изцяло отделен от `template <typename T>` на класа. Така `Pair<int>` и `Pair<double>` остават различни типове, но можем да конвертираме между тях.

> **Правило:** Member template-ите дават гъвкавост при преобразуване между различни инстанции на същия шаблон. Точно така `std::shared_ptr<Derived>` може да се присвои на `std::shared_ptr<Base>`.

---

## Пълна специализация

Понякога за **конкретен тип** общата реализация е неподходяща. Тогава пишем **специализация** — отделна версия на шаблона само за него.

**Пълна специализация** (full specialization) задава всички параметри:

```cpp
template <typename T>
struct TypeName {
    static const char* get() { return "unknown"; }
};

// full specialization for int
template <>
struct TypeName<int> {
    static const char* get() { return "int"; }
};

// full specialization for double
template <>
struct TypeName<double> {
    static const char* get() { return "double"; }
};

TypeName<int>::get();    // "int"
TypeName<double>::get(); // "double"
TypeName<char>::get();   // "unknown" — general template
```

Синтаксисът `template <>` (празни ъглови скоби) сигнализира, че всички параметри са фиксирани. Компилаторът предпочита специализацията пред общия шаблон.

Класически пример от стандартната библиотека е `std::vector<bool>` — специализиран да пакетира булевите стойности побитово вместо побайтово.

---

## Частична специализация

**Частичната специализация** (partial specialization) фиксира **част** от параметрите или налага ограничение върху тях, оставяйки други свободни. Достъпна е само за **класове**, не за функции.

Пример — специализация за указателни типове:

```cpp
template <typename T>
struct Wrapper {
    static void describe() { /* value type */ }
};

// partial specialization for any pointer type T*
template <typename T>
struct Wrapper<T*> {
    static void describe() { /* pointer type */ }
};

Wrapper<int>    w1;   // general version
Wrapper<int*>   w2;   // partial specialization — T = int
Wrapper<double*> w3;  // partial specialization — T = double
```

Друг типичен случай — фиксиране на един от няколко параметъра:

```cpp
template <typename A, typename B>
struct Converter { /* general case */ };

// partial specialization: when both types are the same
template <typename A>
struct Converter<A, A> { /* same-type shortcut */ };

Converter<int, double> c1;   // general
Converter<int, int>    c2;   // partial specialization
```

> **Правило:** Пълна специализация (`template <>`) фиксира **всички** параметри; частична (`template <...>` с по-малко свобода) фиксира **част** от тях или налага структура. Частичната специализация е достъпна само за класове.

---

## Зависими имена: `typename` и `template`

Когато вътре в шаблон обръщаме към име, което **зависи** от шаблонния параметър, компилаторът по подразбиране приема, че то **не** е тип. Ако наистина е тип, трябва изрично да го обявим с `typename`:

```cpp
template <typename Container>
void printFirst(const Container& c) {
    // Container::value_type depends on the template parameter → typename required
    typename Container::value_type first = c.front();
    // ...
}
```

Без `typename` компилаторът би третирал `Container::value_type` като статична членова променлива и би дал грешка.

Аналогично, при извикване на **шаблонен метод** през зависим обект се налага ключовата дума `template`:

```cpp
template <typename T>
void f(T& obj) {
    obj.template get<int>();   // get is a template method of a dependent type
}
```

> **Правило:** Пред зависим **тип** пишем `typename`; пред зависим **шаблонен член** пишем `template`. Това са едни от най-объркващите, но задължителни синтактични изисквания при работа с шаблони.

---

## Статични член-данни в шаблони

Всяка инстанция на шаблон има **свои собствени** статични членове — те не се споделят между различните инстанции:

```cpp
template <typename T>
struct Counter {
    static int count;
    Counter() { ++count; }
};

// definition of the static member (must be in the header)
template <typename T>
int Counter<T>::count = 0;

Counter<int>    a, b;   // Counter<int>::count  == 2
Counter<double> c;      // Counter<double>::count == 1  — separate counter
```

`Counter<int>::count` и `Counter<double>::count` са две **различни** променливи. Дефиницията на статичния член е шаблонна и обикновено стои в хедъра (вж. следващата секция).

---

## Приятелски функции в шаблонни класове

Когато искаме да предефинираме оператор (например `<<`) като приятелска функция, най-простият подход е да го дефинираме **вътре** в тялото на класа:

```cpp
template <typename T>
class Pair {
    T first;
    T second;

public:
    Pair(T a, T b) : first(a), second(b) {}

    // friend function defined inside the class body — one per instantiation
    friend std::ostream& operator<<(std::ostream& os, const Pair<T>& p) {
        return os << '(' << p.first << ", " << p.second << ')';
    }
};

Pair<int> p{1, 2};
std::cout << p;   // (1, 2)
```

За всяка инстанция на `Pair<T>` се генерира съответстваща не-шаблонна `operator<<`. Това е и най-малко проблемният вариант — избягва тънкостите при свързване на шаблонна приятелска функция към шаблонен клас.

По-сложният вариант е да дефинираме `operator<<` като отделна шаблонна функция и да я обявим за приятелска:

```cpp
template <typename T>
class Pair {
    T first;
    T second;
public:
    Pair(T a, T b) : first(a), second(b) {}

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Pair<U>& p);
};

template <typename U>
std::ostream& operator<<(std::ostream& os, const Pair<U>& p) {
    return os << '(' << p.first << ", " << p.second << ')';
}
```

---

## Разделна компилация и шаблони

Това е едно от най-важните практически правила. Шаблоните на **функции** и **класове** не могат да се разделят между `.hpp` и `.cpp` по обичайния начин.

Причината: шаблонът е рецепта. Компилаторът генерира конкретен код едва когато види инстанциране (`Stack<int>`). Ако дефинициите са в отделен `.cpp` файл, компилаторът на него **не знае** за какви типове трябва да инстанцира — и не генерира нищо. При свързването (linking) се получава грешка `undefined reference`:

```cpp
// stack.hpp
template <typename T>
class Stack {
public:
    void push(const T& value);
};

// stack.cpp  — WRONG: definitions in a separate .cpp
template <typename T>
void Stack<T>::push(const T& value) { /* ... */ }

// main.cpp
#include "stack.hpp"
Stack<int> s;
s.push(1);   // linker error: undefined reference to Stack<int>::push
```

**Решение:** дефинициите на шаблонните методи се поставят в хедъра (`.hpp`), заедно с декларацията. Честа конвенция за четимост е разделяне на два файла — декларация в `.hpp` и реализация в `.ipp` / `.tpp`, включван в края на хедъра:

```cpp
// stack.hpp
template <typename T>
class Stack {
public:
    void push(const T& value);
};

#include "stack.ipp"   // bring in the definitions
```

> **Правило:** Целият шаблон (декларация **и** дефиниции) трябва да е видим в точката на инстанциране. На практика: шаблонният код стои в хедъри, не в отделни `.cpp` файлове.

Рядко използвана алтернатива е **явното инстанциране** в `.cpp` файла, когато предварително знаем всички нужни типове:

```cpp
// stack.cpp
#include "stack.hpp"
template class Stack<int>;    // explicit instantiation
template class Stack<double>;
```

---

## Задачи

**Задача 1 – Шаблонен `Pair`**

Реализирайте шаблонен клас `Pair<T>` с две член-данни `first` и `second`, конструктор, гетъри и предефиниран `operator<<` за извеждане във формат `(a, b)`. Тествайте с `Pair<int>`, `Pair<std::string>` и `Pair<double>`.

---

**Задача 2 – Шаблонен стек с динамична памет**

Реализирайте шаблонен клас `Stack<T>` с динамично заделяне на памет (с удвояване на капацитета при нужда). Имплементирайте `push`, `pop`, `top`, `empty`, `size`, Голямата четворка и семантика на преместване. Разделете декларацията и реализацията в `stack.hpp` и `stack.ipp`, като вторият се включва в края на първия.

---

**Задача 3 – `Array<T, N>` с не-типов параметър**

Реализирайте шаблон `Array<T, N>`, обвиващ масив с фиксиран размер `N`, известен при компилация. Добавете `operator[]` (const и не-const), `size()` и проверка за изход извън границите. Покажете, че `Array<int, 3>` и `Array<int, 5>` са различни типове и не могат да се присвояват.

---

**Задача 4 – Конструктор за преобразуване (member template)**

Към `Pair<T>` от Задача 1 добавете member template конструктор, приемащ `Pair<U>` с друг тип `U`. Демонстрирайте конвертиране от `Pair<int>` към `Pair<double>` и обратно.

---

**Задача 5 – Специализация**

Реализирайте шаблон `TypeInfo<T>` със статичен метод `name()`, връщащ `"unknown"`. Добавете пълни специализации за `int`, `double` и `char`. След това добавете частична специализация за всеки указателен тип `T*`, връщаща `"pointer to ..."` (конкатенирана с `TypeInfo<T>::name()`). Тествайте с `TypeInfo<int>`, `TypeInfo<int*>`, `TypeInfo<double*>` и `TypeInfo<float>`.

---

**Задача 6 – `typename` в действие**

Напишете шаблонна функция `sum`, приемаща произволен STL-контейнер и връщаща сумата на елементите му, използвайки `typename Container::value_type` като тип на акумулатора. Тествайте с `std::vector<int>`, `std::vector<double>` и `std::list<int>`.
