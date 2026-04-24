# Предефиниране на оператори – по-дълбоки теми. Специални оператори. Използване.

## Съдържание
- [Предефиниране на оператори – по-дълбоки теми. Специални оператори. Използване.](#предефиниране-на-оператори--по-дълбоки-теми-специални-оператори-използване)
  - [Съдържание](#съдържание)
  - [Оператори за преобразуване на типове](#оператори-за-преобразуване-на-типове)
    - [Имплицитни преобразувания и неочаквани ефекти](#имплицитни-преобразувания-и-неочаквани-ефекти)
    - [Верига от преобразувания](#верига-от-преобразувания)
    - [Задача](#задача)
  - [operator bool()](#operator-bool)
    - [Защо `operator bool()`?](#защо-operator-bool)
    - [Имплицитно `operator bool()` – проблемите](#имплицитно-operator-bool--проблемите)
    - [Модерното решение: `explicit operator bool()` (C++11)](#модерното-решение-explicit-operator-bool-c11)
    - [`operator bool()` за умен указател](#operator-bool-за-умен-указател)
    - [Обобщение: контекстуални преобразувания](#обобщение-контекстуални-преобразувания)
    - [Задача](#задача-1)
  - [explicit оператори за преобразуване](#explicit-оператори-за-преобразуване)
    - [Сравнение: implicit срещу explicit преобразувания](#сравнение-implicit-срещу-explicit-преобразувания)
  - [Прокси обекти и operator\[\]](#прокси-обекти-и-operator)
    - [Мотивиращ пример: пакетиран масив от битове](#мотивиращ-пример-пакетиран-масив-от-битове)
  - [Оператори за управление на памет](#оператори-за-управление-на-памет)
    - [Клас-специфичен new/delete](#клас-специфичен-newdelete)
    - [Масивни форми](#масивни-форми)
    - [Задача](#задача-2)
  - [Оператор -\>](#оператор--)
    - [Семантика на рекурсивното прилагане](#семантика-на-рекурсивното-прилагане)
  - [Практически насоки: кога да предефинирате](#практически-насоки-кога-да-предефинирате)
    - [Обобщение: кои оператори как се дефинират](#обобщение-кои-оператори-как-се-дефинират)
    - [Задача](#задача-3)

---

## Оператори за преобразуване на типове

C++ позволява да дефинирате **как вашият клас се преобразува към друг тип**. Тези оператори нямат върнат тип в сигнатурата – той е закодиран в името им.

```cpp
class Fraction {
    int num, den;
public:
    Fraction(int n, int d) : num(n), den(d) {}

    // Conversion to double: called when Fraction is used where double is expected
    operator double() const {
        return static_cast<double>(num) / den;
    }

    // Conversion to int: truncates towards zero
    operator int() const {
        return num / den;
    }
};

Fraction f(7, 2);
double d = f;        // calls operator double(), d == 3.5
int    i = f;        // calls operator int(),    i == 3

double result = f + 1.5;   // f is implicitly converted to double, then added
```

### Имплицитни преобразувания и неочаквани ефекти

Имплицитните преобразувания могат да доведат до **изненадващо поведение** – компилаторът прилага ги автоматично, понякога там, където не очаквате:

```cpp
class MyArray {
    int* data;
    size_t sz;
public:
    MyArray(size_t n) : sz(n), data(new int[n]()) {}
    ~MyArray() { delete[] data; }

    // Dangerous: implicit conversion to pointer
    operator int*() { return data; }
};

MyArray a(10), b(20);

// Intended: some custom comparison
// Actual: both arrays implicitly convert to int*, then raw pointers are compared!
if (a == b) { /* this compares memory addresses, not contents */ }

// Even worse: silently allows pointer arithmetic on the internal buffer
int* p = a;
p[100] = 42;   // out-of-bounds write – undefined behaviour
```

> **Правило:** Имплицитните оператори за преобразуване трябва да се дефинират **предпазливо**. Когато има съмнение – използвайте `explicit`.

### Верига от преобразувания

Компилаторът може да приложи **най-много едно** имплицитно потребителско преобразуване на стъпка. Ако са нужни две – компилацията се проваля:

```cpp
class Celsius {
    double temp;
public:
    Celsius(double t) : temp(t) {}
    operator double() const { return temp; }
};

class Fahrenheit {
    double temp;
public:
    Fahrenheit(double t) : temp(t) {}
    // Implicit construction from double is allowed (no explicit)
};

Celsius c(100.0);
// Fahrenheit f = c;
// ERROR: would need Celsius→double→Fahrenheit (two user-defined conversions)
```

### Задача

Към класа `my_std::basic_string<char>` добавете:

- `operator const char*() const` – връща вътрешния буфер като `const char*`, позволявайки низа да се използва там, където се очаква C-низ. Уверете се, че буферът е нулево-терминиран.
- `operator bool() const` – разгледан подробно в следващия раздел.

---

## operator bool()

### Защо `operator bool()`?

Много класове имат естествено понятие за "валидност" или "непразност" – файлов поток, умен указател, буфер. Желаем да можем да пишем:

```cpp
if (stream) { /* stream is open and in good state */ }
if (ptr)    { /* pointer is not null */ }
if (str)    { /* string is not empty */ }
```

### Имплицитно `operator bool()` – проблемите

Преди C++11 дефинирането на `operator bool()` без `explicit` водеше до **сериозни неочаквани преобразувания**:

```cpp
class Stream {
public:
    // Non-explicit: dangerous
    operator bool() const { return isOpen(); }
private:
    bool isOpen() const { return /* ... */ true; }
};

Stream s1, s2;

// Intended: compare two streams somehow
// Actual: both convert to bool (0 or 1), then integers are compared!
if (s1 == s2) { /* almost certainly wrong */ }

// Intended: nothing sensible
// Actual: compiles silently – stream converts to bool, then to int
int n = s1 + s2;   // n is 0, 1, or 2 depending on which streams are open
```

### Модерното решение: `explicit operator bool()` (C++11)

Ключовата дума `explicit` забранява **имплицитното** преобразуване, но позволява **контекстуалното** – т.е. в `if`, `while`, `&&`, `||`, `!` и тернарния оператор:

```cpp
class FileHandle {
    int fd;         // file descriptor; -1 means closed
public:
    explicit FileHandle(const char* path) : fd(/* open */ 0) {}
    ~FileHandle() { if (fd >= 0) { /* close */ } }

    // explicit: disables implicit conversions to bool
    // but contextual conversion still works in boolean contexts
    explicit operator bool() const {
        return fd >= 0;
    }
};

FileHandle f("data.txt");

if (f) { /* OK – contextual conversion in boolean context */ }
bool b = static_cast<bool>(f);   // OK – explicit cast

// bool b = f;     // ERROR – implicit conversion is blocked by explicit
// int n = f + 1;  // ERROR – cannot implicitly convert to bool, then to int
// if (f == f) {}  // ERROR – no operator== defined
```

### `operator bool()` за умен указател

Класически пример – умен указател трябва да се държи като указател в булев контекст:

```cpp
template<typename T>
class UniquePtr {
    T* ptr;
public:
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}
    ~UniquePtr() { delete ptr; }

    // Disable copy
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Allow move
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }

    T& operator*()  const { return *ptr; }
    T* operator->() const { return ptr; }

    // Allows: if (ptr) { ... }   while (ptr) { ... }   bool b = !ptr;
    explicit operator bool() const { return ptr != nullptr; }
};

UniquePtr<int> p(new int(42));
if (p) {
    *p = 100;   // safe dereference
}

UniquePtr<int> empty;
if (!empty) {
    // correctly detected as null
}
```

### Обобщение: контекстуални преобразувания

Местата, където `explicit operator bool()` се прилага **автоматично**:

```cpp
explicit operator bool() const { return /* ... */ true; }

// All of these trigger contextual conversion – they work:
if (obj) {}
while (obj) {}
bool b = !obj;
bool c = obj && other;
bool d = obj || other;
auto x = obj ? 1 : 0;

// These require explicit cast or are errors:
// bool b = obj;            // ERROR – implicit
// int n = obj;             // ERROR – implicit
// if (obj == true) {}      // ERROR – no operator==
```

### Задача

Към класа `my_std::basic_string<T>` добавете:

- `explicit operator bool() const` – връща `true` ако низът е ненулев и непразен. Трябва да работи в: `if (str)`, `while (str)`, `!str`, `str && other`.
- Проверете, че следното **не се компилира** (напишете го като коментар в теста):
  ```cpp
  // basic_string<char> s("hello");
  // int n = s;        // must not compile
  // bool b = s;       // must not compile (implicit)
  ```

---

## explicit оператори за преобразуване

Ключовата дума `explicit` може да се приложи към **всеки** оператор за преобразуване, не само `operator bool()`. Това е особено важно когато искате да разрешите явното преобразуване, но да забраните мълчаливото.

```cpp
class Decibels {
    double value;
public:
    explicit Decibels(double v) : value(v) {}

    // Explicit: must cast manually – prevents accidental mixing of units
    explicit operator double() const { return value; }
};

class Watts {
    double value;
public:
    explicit Watts(double v) : value(v) {}
    explicit operator double() const { return value; }
};

Decibels db(30.0);
Watts    w(1.0);

double d = static_cast<double>(db);   // OK – explicit cast
// double d2 = db;                    // ERROR – implicit blocked
// double sum = db + w;               // ERROR – units cannot be silently mixed
```

### Сравнение: implicit срещу explicit преобразувания

| | `operator T()` | `explicit operator T()` |
|:---|:---:|:---:|
| В `if` / `while` / `&&` / `\|\|` | ✅ | ✅ |
| Имплицитно при инициализация (`T x = obj`) | ✅ | ❌ |
| Имплицитно при предаване на аргумент | ✅ | ❌ |
| Явно с `static_cast<T>` | ✅ | ✅ |
| Верижно (две конверсии на стъпка) | ✅ (ако и двете са implicit) | ❌ |

> **Добра практика:** По подразбиране пишете `explicit` на операторите за преобразуване. Премахвате го само когато преобразуването е очевидно безопасно и семантично коректно (напр. `Celsius` → `double`).

---

## Прокси обекти и operator[]

Понякога `operator[]` не може просто да върне референция – трябва да **прихване** и четенето, и записването поотделно. Решението е **прокси обект** (proxy object): `operator[]` връща временен обект, чийто `operator=` и оператор за преобразуване вършат реалната работа.

### Мотивиращ пример: пакетиран масив от битове

Клас, съхраняващ битове плътно (по 8 в байт), не може да върне `bool&` – отделен бит не може да се адресира директно. Нуждаем се от прокси:

```cpp
class BitArray {
    unsigned char* bytes;
    size_t bit_count;

    // Forward declaration
    class BitProxy;

public:
    BitArray(size_t n)
        : bit_count(n)
        , bytes(new unsigned char[(n + 7) / 8]())
    {}

    ~BitArray() { delete[] bytes; }

    // --- Proxy class ---
    class BitProxy {
        unsigned char& byte;   // reference to the byte containing the bit
        unsigned char  mask;   // bitmask for the specific bit within the byte

    public:
        BitProxy(unsigned char& b, unsigned char m) : byte(b), mask(m) {}

        // Called on write: arr[i] = true
        BitProxy& operator=(bool value) {
            if (value) byte |=  mask;   // set bit
            else       byte &= ~mask;   // clear bit
            return *this;
        }

        // Called on read: bool b = arr[i]
        operator bool() const {
            return (byte & mask) != 0;
        }
    };

    // operator[] returns a proxy, not a direct reference
    BitProxy operator[](size_t index) {
        return BitProxy(bytes[index / 8], 1u << (index % 8));
    }

    // Const version: plain bool is enough (read-only)
    bool operator[](size_t index) const {
        return (bytes[index / 8] & (1u << (index % 8))) != 0;
    }
};

BitArray flags(16);
flags[0] = true;    // calls BitProxy::operator=(bool)
flags[3] = true;

bool b = flags[0];  // calls BitProxy::operator bool()
bool c = flags[1];  // false

const BitArray& ref = flags;
bool d = ref[0];    // calls const operator[] → plain bool
```

> **Кога да използвате прокси:** Когато `operator[]` трябва да различи четене от запис, или когато елементите не съществуват като отделни адресируеми обекти.

---

## Оператори за управление на памет

C++ позволява да предефинирате `operator new` и `operator delete` – и глобално, и за конкретен клас. Това е полезно за: **пул алокатори**, **диагностика на изтичания на памет**, **изравняване** (alignment), и **вграден (embedded) код** без стандартен heap.

### Клас-специфичен new/delete

Операторите, дефинирани вътре в клас, се прилагат само при алокация на **обекти от този клас**:

```cpp
#include <cstdlib>
#include <cstdio>

class Tracked {
    static size_t total_allocated;

public:
    int value;

    // Called instead of global ::operator new when allocating Tracked
    static void* operator new(size_t size) {
        total_allocated += size;
        std::printf("[alloc] %zu bytes (total: %zu)\n", size, total_allocated);
        void* ptr = std::malloc(size);
        if (!ptr) throw std::bad_alloc();
        return ptr;
    }

    // Called instead of global ::operator delete when freeing Tracked
    static void operator delete(void* ptr, size_t size) {
        total_allocated -= size;
        std::printf("[free] %zu bytes (total: %zu)\n", size, total_allocated);
        std::free(ptr);
    }

    static size_t get_allocated() { return total_allocated; }
};

size_t Tracked::total_allocated = 0;

Tracked* t = new Tracked();     // calls Tracked::operator new
delete t;                       // calls Tracked::operator delete
```

### Масивни форми

За `new[]` и `delete[]` са нужни отделни предефинирания:

```cpp
class PoolObject {
public:
    static void* operator new[](size_t size) {
        // allocate from a memory pool instead of the heap
        return std::malloc(size);
    }

    static void operator delete[](void* ptr) {
        std::free(ptr);
    }
};
```

### Задача

Към класа `my_std::basic_string<T>` добавете **диагностичен** `operator new` и `operator delete`:

- `static void* operator new(size_t size)` – отпечатва `"[basic_string] alloc N bytes"` и извиква `std::malloc`.
- `static void operator delete(void* ptr)` – отпечатва `"[basic_string] free"` и извиква `std::free`.
- Проверете, че при `new basic_string<char>("test")` и последващ `delete` диагностичните съобщения се появяват правилно.

---

## Оператор ->

Операторът `->` е специален: компилаторът го прилага **рекурсивно**, докато резултатът е суров указател или обект без `operator->`. Това позволява верижното `->` в итератори и обвивки.

### Семантика на рекурсивното прилагане

```cpp
obj->member
// Step 1: call obj.operator->()  → returns ptr_or_obj
// Step 2: if ptr_or_obj is a pointer: (ptr_or_obj)->member
//         if ptr_or_obj has operator->: repeat from step 1
```

Това означава, че можете да вържете произволен брой нива на обвивки:

```cpp
struct Inner {
    int value = 42;
};

struct MiddleWrapper {
    Inner inner;
    Inner* operator->() { return &inner; }
};

struct OuterWrapper {
    MiddleWrapper mw;
    MiddleWrapper& operator->() { return mw; }
    // Not actually correct – operator-> must return a pointer or something
    // with operator->. Let's use a pointer:
    MiddleWrapper* operator->_ptr() { return &mw; }
};

// More realistic: smart-pointer chain
template<typename T>
struct Logged {
    T* ptr;
    explicit Logged(T* p) : ptr(p) {}

    T* operator->() const {
        std::printf("[access via Logged]\n");
        return ptr;   // raw pointer – chain ends here
    }
};

Logged<Inner> obj(new Inner());
int v = obj->value;   // prints "[access via Logged]", then accesses Inner::value
delete obj.ptr;
```

---

## Практически насоки: кога да предефинирате

**Предефинирайте оператор когато:**

- Значението му е **очевидно** от домейна (`Matrix +`, `Complex *`, `String +`).
- Операторът следва **очакваната математическа семантика** (комутативност, асоциативност).
- Алтернативата – именувана функция – е значително по-многословна за потребителя.

**Не предефинирайте оператор когато:**

- Смисълът му е **неясен** (`File * File`, `Thread + Thread`).
- Операторът би **нарушил принципа на най-малката изненада** – потребителите ще очакват грешна семантика.
- Само искате да спестите типизиране – предпочетете именуван метод.

### Обобщение: кои оператори как се дефинират

| Оператор | Форма | Връща | Бележка |
|:---|:---|:---|:---|
| `=`, `[]`, `()`, `->` | Член | Зависи | Задължително член |
| `+=`, `-=`, `*=` и др. | Член | `T&` (self) | Член за консистентност |
| `+`, `-`, `*` и др. | Свободна | `T` (нова стойност) | Изведена от `op=` |
| `==`, `!=`, `<` и др. | Свободна | `bool` | Свободна за симетрия |
| `<<`, `>>` | Свободна | `stream&` | Трябва `friend` |
| `++`, `--` (prefix) | Член | `T&` | Без параметри |
| `++`, `--` (postfix) | Член | `T` (копие) | Фиктивен `int` |
| `operator T()` | Член | – (в името) | Добавете `explicit` |
| `operator bool()` | Член | `bool` | Винаги `explicit` |
| `new`, `delete` | Статичен член | `void*` / `void` | За клас-специфична памет |

### Задача

Реализирайте клас `StringBuilder`, който събира низове ефективно чрез предефинирани оператори:

- Вътрешно използвайте `my_std::basic_string<char>` като буфер.
- `StringBuilder& operator<<(const char* str)` – добавя C-низ в края на буфера.
- `StringBuilder& operator<<(char ch)` – добавя единичен символ.
- `StringBuilder& operator<<(int n)` – конвертира цялото число до символи и ги добавя (реализирайте ръчно без `sprintf`).
- `explicit operator bool() const` – `true` ако буферът не е празен.
- `const my_std::basic_string<char>& str() const` – достъп до натрупания резултат.

Примерна употреба:

```cpp
StringBuilder sb;
sb << "Player " << 42 << " scored " << 100 << " points.";
if (sb) {
    // print sb.str()
}
```