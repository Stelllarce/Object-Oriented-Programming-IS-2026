# Жизнен цикъл на обектите. Изключения. Rule of Zero/Four.

---

## Съдържание

1. [Специални член-функции](#специални-член-функции)
2. [Конструктор за копиране и оператор за копиращо присвояване](#конструктор-за-копиране-и-оператор-за-копиращо-присвояване)
3. [Rule of Zero](#rule-of-zero)
4. [Rule of Four](#rule-of-four)
5. [Изключения](#изключения)

---

## Специални член-функции

C++ автоматично генерира (при нужда) шест специални член-функции за всеки клас. Те управляват **жизнения цикъл** на обектите – как се създават, копират, преместват и унищожават.

| Функция | Сигнатура | Кога се извиква |
|:--------|:----------|:----------------|
| Конструктор по подразбиране | `T()` | При създаване без аргументи |
| Деструктор | `~T()` | При излизане от обхват или `delete` |
| Конструктор за копиране | `T(const T&)` | При инициализация от съществуващ обект |
| Оператор за копиращо присвояване | `T& operator=(const T&)` | При присвояване на съществуващ обект |
| Конструктор за преместване | `T(T&&)` | При инициализация от rvalue |
| Оператор за преместващо присвояване | `T& operator=(T&&)` | При присвояване от rvalue |

```cpp
class MyClass {
public:
    MyClass();                            // default constructor
    ~MyClass();                           // destructor

    MyClass(const MyClass&);              // copy constructor
    MyClass& operator=(const MyClass&);   // copy assignment operator

    MyClass(MyClass&&);                   // move constructor
    MyClass& operator=(MyClass&&);        // move assignment operator
};
```

---
> **Важно:** Ако декларирате някоя от тези функции ръчно, компилаторът може да спре да генерира останалите автоматично.

---

## Конструктор за копиране и оператор за копиращо присвояване

Когато клас управлява **динамично заделена памет**, копирането по подразбиране е плитко (shallow copy). Тогава двата обекта ще сочат към един и същ ресурс, и при унищожаване ще се получи double free.

![](https://miro.medium.com/v2/resize:fit:720/format:webp/1*ZDBlNPFa1v4IR7VGnJA3Fg.jpeg)

```cpp
class DynamicArray {
    int* data;
    int size;

public:
    DynamicArray(int n) : size(n), data(new int[n]) { std::cout << "Constructor\n"; }
    ~DynamicArray() { 
        std::cout << "Destructor\n";
        delete[] data; 
    }
};

int main() {
    DynamicArray a(5);
    DynamicArray b = a;    // copy constructor – shallow copy!
    // Both a and b point to the same data – double free when destructors run, because both object are being destroyed at the same time...
} //<- ...here
```

```bash
Constructor
Destructor
Destructor
free(): double free detected in tcache 2
Aborted (core dumped)
```

За да се справим с това, трябва да дефинираме **copy constructor** и **copy assignment operator**, които правят **дълбоко копие** (deep copy) на ресурса.

```cpp
class DynamicArray {
    int* data = nullptr;
    size_t size = 0;

    void copy(const DynamicArray& other) {
        size = other.size;
        data = new int[size];
        for (int i = 0; i < size; i++)
            data[i] = other.data[i];
    }

public:
    DynamicArray(int n) : size(n), data(new int[n]) {}

    // Without a proper copy constructor, this would copy only the pointer!

    // Copy constructor – deep copy
    DynamicArray(const DynamicArray& other) {
        copy(other);
    }

    // Copy assignment operator – deep copy with self-assignment guard
    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this; // if we are assigning the object to itself, do nothing (e.g. a = a;)

        delete[] data; // free old resource
        copy(other);

        return *this; // Done to allow chaining (e.g. a = b = c);
    }

    ~DynamicArray() { delete[] data; }
};
```

### Кога се извикват

Ако обектът не е създаден предварително **ВИНАГИ** се извиква конструкторът за копиране, дори и ако го създадем с `=` (това е инициализация, не присвояване). Ако обектът вече съществува, се извиква операторът за копиращо присвояване.

```cpp
DynamicArray a(5);
DynamicArray b = a;    // copy constructor  (initialization)
DynamicArray c(a);     // copy constructor  (explicit)
b = c;                 // copy assignment   (b already exists)
```

### Начини да пишем конструктра за копиране и оператор за копиращо присвояване

Нека за примера използваме следния клас:

```cpp
class Mock {
public:
    Mock(size_t n) : size(n), data(new int[n]{}) {}
    ~Mock() { delete[] data; }

    // --- copy constructors shown below ---
private:
    // Alternatively, use a copy helper function to avoid code duplication
    void copy(const Mock& other) {
        size = other.size;
        data = new int[size];
        for (size_t i = 0; i < size; ++i)
            data[i] = other.data[i];
    }

    size_t size;
    int* data;
};
```

#### Цикъл

```cpp
Mock(const Mock& other) : size(other.size), data(new int[size]) {
    // copy(other) // alternative to avoid code duplication
    for (size_t i = 0; i < size; ++i)
        data[i] = other.data[i];
}

Mock& operator=(const Mock& other) {
    if (this == &other) return *this;

    delete[] data;
    // Change below alternatively
    size = other.size;
    data = new int[size];

    for (size_t i = 0; i < size; ++i)
        data[i] = other.data[i];
    // Change above alternatively
    // ...to this
    // copy(other);
    return *this;
}
```

#### Copy-and-swap

```cpp
Mock(const Mock& other) : size(other.size), data(new int[size]) {
    for (size_t i = 0; i < size; ++i)
        data[i] = other.data[i];
}

Mock& operator=(const Mock& other) {
    if (this == &other) return *this;

    Mock temp(other); // temporary object

    std::swap(data, tmp.data)
    std::swap(size, tmp.size)

    return *this;
} // temp gets destroyed here, but it has the old resource, so no double free
```

### `= delete` – изключване на автоматично генерираните функции

Ако не искаме дадена функция да се генерира автоматично от компилатора, можем да я маркираме с `= delete`. Това ще предотврати компилирането, ако някой се опита да я използва.

```cpp
class NonCopyable {
public:
    NonCopyable() = default; // allow default constructor
    NonCopyable(const NonCopyable&) = delete; // disable copy constructor
    NonCopyable& operator=(const NonCopyable&) = delete; // disable copy assignment
};
```

```cpp
NonCopyable a;
NonCopyable b = a; // error: use of deleted function
```

---

## Rule of Zero

**Ако класът не управлява ресурси директно – не дефинирайте нито една специална член-функция.**

Когато всички член-данни са стойностни типове или RAII обвивки (напр. `std::string`, `std::vector`, `std::unique_ptr`), компилаторът генерира правилни версии на всички шест функции автоматично.

```cpp
// GOOD: Rule of Zero – the compiler handles everything correctly
class Student {
    std::string name;       // std::string manages its own memory
    int age;
    std::vector<int> grades; // std::vector manages its own memory

public:
    Student(std::string n, int a) : name(std::move(n)), age(a) {}

    // No destructor, no copy/move – compiler generates them all correctly
};
```

```cpp
// BAD: raw pointer forces us to manage memory manually
class Student {
    char* name;   // now we MUST write copy, move, and destructor
    int age;
};
```

> **Правилото:** Ако можете да следвате Rule of Zero – следвайте го. Използвайте RAII типове и избягвайте `new`/`delete` в класове.

---

## Голяма четворка (Rule of Three)


Ако класът **трябва** да управлява ресурс директно (напр. `new`/`delete`, файлове, мрежови сокети) и деструкторът се грижи за освобождаването, трябва да дефинирате четири функции:

- **Конструктоп по подразбиране**
- **Деструктор**
- **Конструктор за копиране**
- **Оператор за копиращо присвояване**

> В чужди източници му викат Rule of Three, защото не броят конструктора по подразбиране

### Упражнение: `my_std::basic_string<T>` и Rule of Four

Добавете към `my_std::basic_string<T>` следните член-функции:

- Конструктор за копиране с дълбоко копие на буфера.
- Оператор за копиращо присвояване, коректен при self-assignment.

---

## Изключения

Изключенията са механизъм за **обработка на грешки по време на изпълнение**. Когато нещо се обърка, хвърляме (`throw`) изключение, което прекъсва нормалното изпълнение и се прехваща (`catch`) на подходящо място. Ако не бъде прихванато, програмата се прекратява. Имаме следните нови ключови думи:

- `try` - дефинира блок, в който може да се хвърли изключение. Изключението може да бъде хвърлено както от функции директно викнати в блока, така и от функции, извикани от други функции.
- `catch` - дефинира блок, който обработва изключения от определен тип. Като параметър се посочва типа на изключението, което искаме да прихванем. Ако искаме да прихванем всички изключения, можем да използваме `catch(...)`. Алтернативно можем да използваме `catch(const std::exception& e)`, за да прихванем всички стандартни изключения.
- `throw` - използва се за хвърляне на изключение. Последвано е от типа на изключението и (опционално) от аргументи, които се предават на конструктора на изключението. Можем и да не хвърляме конкретен обект, а просто `throw;` за да прехвърлим текущото изключение към по-високо ниво.

```cpp
#include <stdexcept>

double divide(double a, double b) {
    if (b == 0.0)
        throw std::invalid_argument("Division by zero is not allowed");
    return a / b;
}

int main() {
    try {
        double result = divide(10.0, 0.0);
    }
    catch (const std::invalid_argument& e) {
        // e.what() returns the error message
        std::cerr << "Error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // catch-all for any other standard exception
        std::cerr << "Standard error: " << e.what() << std::endl;
    }
    catch (...) {
        // catch-all – handles any other exception type
        std::cerr << "Unknown error occurred" << std::endl;
    }
}
```

### Стандартна йерархия на изключенията

```
std::exception
├── std::logic_error
│   ├── std::invalid_argument
│   ├── std::out_of_range
│   └── std::length_error
└── std::runtime_error
    ├── std::overflow_error
    ├── std::underflow_error
    └── std::range_error
```

Всички стандартни изключения наследяват `std::exception` и имплементират метода `what()`, който връща описание на грешката.

### Собствени типове изключения

```cpp
#include <stdexcept>
#include <string>

// Custom exception – inherits from std::runtime_error
class DatabaseError : public std::runtime_error {
    int errorCode;

public:
    DatabaseError(const std::string& message, int code)
        : std::runtime_error(message), errorCode(code) {}

    int getCode() const { return errorCode; }
};

void connectToDatabase(const std::string& url) {
    if (url.empty())
        throw DatabaseError("Empty connection URL", 1001);
}

int main() {
    try {
        connectToDatabase("");
    }
    catch (const DatabaseError& e) {
        std::cerr << "DB Error [" << e.getCode() << "]: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        // Catches any other standard exception
        std::cerr << "Standard error: " << e.what() << std::endl;
    }
}
```

### Важни правила при хвърляне и прихващане

```cpp
// GOOD: catch by const reference – avoids copying, preserves polymorphism
catch (const std::exception& e) { ... }

// BAD: catch by value – slices the object if it's a subclass
catch (std::exception e) { ... }

// BAD: catch by pointer – requires manual memory management
catch (std::exception* e) { ... }
```

```cpp
// Catch blocks are evaluated in order – put more specific types FIRST
try { ... }
catch (const std::invalid_argument& e) { ... }   // more specific – first
catch (const std::logic_error& e) { ... }         // less specific – second
catch (const std::exception& e) { ... }           // most general – last
```

### Re-throw

```cpp
void process() {
    try {
        riskyOperation();
    }
    catch (const std::exception& e) {
        // Log the error, then re-throw to let the caller handle it
        std::cerr << "Logging: " << e.what() << std::endl;
        throw;   // re-throws the current exception without slicing
    }
}
```


### Изключения и жизнен цикъл на обектите

Когато се хвърли изключение, стекът се **развива** (stack unwinding) – всички локални обекти се унищожават в обратен ред на създаването им. Деструкторите **се извикват гарантирано**, дори при изключение.

![](https://i.sstatic.net/miFv3.png)

Точно това е принципът зад **RAII** – ресурсът се освобождава в деструктора, а деструкторът се извиква винаги, независимо от изключения.

### Изключения в конструктори

Ако конструкторът хвърли изключение, деструкторът на обекта **не се извиква** (обектът не е бил напълно създаден). Вече конструираните под-обекти и базови класове обаче **се унищожават нормално**.

```cpp
class ResourceOwner {
    int* first;
    int* second;

public:
    ResourceOwner() {
        first = new int(1);          // allocated successfully
        second = new int(2);         // if THIS throws, first leaks!
        throw std::runtime_error("Constructor failed"); // hypothetical
    }

    ~ResourceOwner() {
        delete first;   // never reached if constructor threw
        delete second;
    }
};
```
Затова е важно или да се възползвате от RAII типове или да хвърляте изключения преди да сте заделили ресурси, за да избегнете изтичане при неуспешно конструиране.

### Деструктори не трябва да хвърлят изключения

По време на stack unwinding се викат деструкторите на обектите. Ако някой от тях хвърли иключение, докато друго такова се обработва, програмата ще се срине (`std::terminate`, което на свой ред вика `std::abort`), защото не може да обработва две изключения едновременно.

```cpp
class SafeResource {
public:
    ~SafeResource() {
        try {
            cleanup();   // cleanup might throw internally
        }
        catch (...) {
            // Swallow all exceptions in the destructor
        }
    }

private:
    void cleanup() { /* ... */ }
};
```

---

## Гаранции при изключения (Exception Safety Guarantees)

При операция, която може да се провали, е важно да знаем **в какво състояние остава програмата**.

### `noexcept` – гаранция за невъзникване на изключение (No-throw Guarantee)

```cpp
void process() noexcept {
    // This function is guaranteed not to throw any exceptions
}
```

При разделна компилация трябва да пишете този идентификатор и в декларацията, и в дефиницията на функцията, за да е валиден.

**Logger.hpp**
```cpp
class Logger {
public:
    void log(const std::string& message) noexcept; // declaration with noexcept
};
```

**Logger.cpp**
```cpp
#include "Logger.hpp"
void Logger::log(const std::string& message) noexcept { // definition with noexcept
    // Implementation that guarantees no exceptions are thrown
}
```

### 1. Слаба гаранция (Weak Guarantee)

Ако операцията се провали, засегнатите обекти остават в **валидно, но неопределено** състояние. Конкретните стойности са неизвестни, но всеки обект може безопасно да бъде унищожен или преизползван - няма изтичане на ресурси.

```cpp
#include <cstddef>
#include <stdexcept>

struct LogLine {
    int code = 0;
};

class Logger {
    LogLine* lines = nullptr;
    std::size_t used = 0;
    std::size_t capacity = 0;
    std::size_t lineCount = 0; // duplicated metadata

    void grow() {
        const std::size_t newCapacity = (capacity == 0) ? 4 : capacity * 2;
        LogLine* bigger = new LogLine[newCapacity]; // may throw std::bad_alloc
        for (std::size_t i = 0; i < used; ++i)
            bigger[i] = lines[i];
        delete[] lines;
        lines = bigger;
        capacity = newCapacity;
    }

    void updateSecondaryIndex() {
        if (lineCount == 3)
            throw std::runtime_error("secondary index write failed");
    }

public:
    ~Logger() { delete[] lines; }

    // Weak guarantee: if updateSecondaryIndex throws,
    // 'used' is already incremented but 'lineCount' is not.
    // The object is still destructible and reusable.
    void append(const LogLine& msg) {
        if (used == capacity)
            grow();              // may throw

        lines[used++] = msg;     // state changed
        updateSecondaryIndex();  // may throw
        ++lineCount;             // metadata update
    }
};
```

> **Какво се гарантира:** Нито един ресурс не изтича. Обектът може да бъде унищожен.
>
> **Какво НЕ се гарантира:** Инвариантите на класа може да са нарушени — `used` и `lineCount` може да се разминат.

---

### 2. Частична garantия (Partial Guarantee)

Ниво между слабата и силната гаранция. Операцията гарантира **запазване на определена, документирана** подмножество от данните — но не непременно всички. Точно какво е запазено трябва изрично да е описано в документацията.

```cpp
#include <cstddef>
#include <stdexcept>

struct Record {
    int value = 0;
};

class RecordBuffer {
    Record*      data = nullptr;
    std::size_t  used = 0;
    std::size_t  capacity = 0;

    void grow() {
        const std::size_t newCapacity = (capacity == 0) ? 8 : capacity * 2;
        Record* bigger = new Record[newCapacity]; // may throw std::bad_alloc
        for (std::size_t i = 0; i < used; ++i)
            bigger[i] = data[i];
        delete[] data;
        data = bigger;
        capacity = newCapacity;
    }

public:
    ~RecordBuffer() { delete[] data; }

    void push_back(const Record& r) {
        if (used == capacity)
            grow();
        data[used++] = r;
    }
};

Record expensiveTransform(const Record& input) {
    if (input.value < 0)
        throw std::runtime_error("invalid record");
    return Record{input.value * 2};
}

// Partial guarantee: if an exception occurs during processing,
// all elements processed *before* the failure are guaranteed to be saved.
// Elements from the point of failure onward are not processed.
void processAndSave(RecordBuffer& output,
                    const Record* input,
                    std::size_t inputCount)
{
    for (std::size_t i = 0; i < inputCount; ++i) {
        Record processed = expensiveTransform(input[i]); // may throw
        output.push_back(processed);                     // may throw
    }
}
```

---

### 3. Силна гаранция (Strong Guarantee)

Ако операцията се провали, всички засегнати обекти се връщат в **точно същото логическо състояние** от преди извикването. Операцията е атомарна: или успява напълно, или все едно не е стартирала.

#### Copy-and-swap

Класическият начин за постигане на силна гаранция при преприсвояване:

```cpp
#include <cstddef>

class DataStore {
    int*         data = nullptr;
    std::size_t  size = 0;
public:
    DataStore() = default;

    DataStore(const DataStore& other)
        : data(new int[other.size]), size(other.size)
    {
        for (std::size_t i = 0; i < size; ++i)
            data[i] = other.data[i];
    }

    ~DataStore() { delete[] data; }

    // Strong guarantee via copy-and-swap:
    // 1. Build a temporary copy — exceptions may occur here
    // 2. swap() is noexcept — it only runs after all risky work is done
    // 3. If any exception is thrown before swap(), *this is left unchanged
    DataStore& operator=(const DataStore& other) {
        DataStore temp(other); // risky copy — may throw
        swap(temp);            // noexcept commit
        return *this;
    }

    // noexcept swap is a prerequisite for the pattern to work
    void swap(DataStore& other) noexcept {
        int* tmpData = data;
        data = other.data;
        other.data = tmpData;

        std::size_t tmpSize = size;
        size = other.size;
        other.size = tmpSize;
    }
};
```

#### Силна гаранция при съставни операции (по-често ще го пишете така)

```cpp
#include <stdexcept>

class Transaction {
    int balance = 0;
public:
    void transfer(int amount) {
        debit(amount);    // step 1: strong guarantee individually; may throw

        try {
            credit(amount); // step 2: may throw
        } catch (...) {
            undoDebit(amount); // return object to original state
            throw;
        }
    }

private:
    void debit(int amount)        { balance -= amount; }  // may throw
    void credit(int amount)       { balance += amount; }  // may throw
    void undoDebit(int n) noexcept { balance += n; }      // noexcept rollback
};
```

---

### 4. Гаранция за непровал (No-fail Guarantee)

Най-силното ниво. Функцията **никога не индикира провал по никакъв начин** — нито чрез изключение, нито чрез върнат код за грешка. Тя безусловно успява (освен при абсолютно невъзстановими ситуации като изчерпване на паметта или хардуерен отказ).

```cpp
#include <cstddef>

class Stack {
    const std::size_t MAX = 128;
    int elems[MAX]{};
    std::size_t topIndex = 0;
public:
    // No-fail guarantee: always returns a valid count — cannot fail by any means
    std::size_t size() const noexcept {
        return topIndex;
    }

    // No-fail guarantee: swap is the unconditional commit step
    // used in higher-level strong-guarantee operations
    void swap(Stack& other) noexcept {
        for (std::size_t i = 0; i < MAX; ++i) {
            int tmp = elems[i];
            elems[i] = other.elems[i];
            other.elems[i] = tmp;
        }

        std::size_t tmpTop = topIndex;
        topIndex = other.topIndex;
        other.topIndex = tmpTop;
    }

    // No-fail guarantee: always succeeds given the precondition (non-empty stack)
    // Precondition violations are UB — the caller is responsible for checking
    void pop() noexcept {
        --topIndex;
    }
};
```

> **Разлика от no-throw:** No-throw е езикова гаранция — изключение не се разпространява навън. No-fail е семантична гаранция — операцията е безусловно успешна. Всяка no-fail функция е и no-throw, но не обратното.
>
> **Кога е необходимо:** Финални commit стъпки, rollback операции, деструктори, `swap`, прости getter-и.

---

### 5. Смесени гаранции (Mixed Guarantees)

Някои операции предоставят различни гаранции за различни условия — например едно конкретно условие е гарантирано без провал, но за останалите данни се дава само слаба или силна гаранция. Такива детайли трябва изрично да се документират.

```cpp
// Mixed guarantee:
// - The log is ALWAYS flushed (no-fail for that specific condition)
// - The cache may be left in an unspecified state on failure (weak guarantee)
void processWithLogging(Cache& cache, Logger& log) {
    try {
        cache.process();   // weak guarantee — may leave cache in unspecified state
    } catch (...) {
        log.flush();       // no-fail — guaranteed to run and succeed regardless
        throw;
    }
    log.flush();           // also runs on success
}
```

---

### Обобщение

| Гаранция | При провал... | Типична употреба |
|---|---|---|
| **Слаба** | Валидно, но неопределено състояние | Липса на манежиране на динамична памет, нямаме проблеми да оставим обекта в невалидно състояние |
| **Частична** | Между силна и слаба | Очакваме да поставим обекта в частично валидно състояние |
| **Силна** | Пълно връщане на обекта в началното състояние | Манежиране на динамична памет |
| **No-throw** | Изключение не се хвърля (езикова) | Деструктори, `swap`, `move` |
| **No-fail** | Провал е невъзможен по какъвто и да е начин | Прости операции, които често не се занимават с управление на ресурси |

> **За изпити**: Научете как да прилагате **Слаба** и **Силна гаранция**. Друг тип гаранции няма да изискваме.

### Упражнение: Изключения

Разширете `my_std::basic_string<T>` със следните методи и коректно хвърляне на изключения:

- `T& front()`, `const T& front() const` – хвърля `std::out_of_range`, ако низът е празен.
- `T& back()`, `const T& back() const` – хвърля `std::out_of_range`, ако низът е празен.
- `void pop_back()` – хвърля `std::underflow_error`, ако низът е празен.
- `void reserve(size_t newCapacity)` – хвърля `std::length_error` при невъзможен/прекомерен капацитет.

### Упражнение: Силна гаранция при изключения

Преработете copy assignment на `my_std::basic_string<T>`, така че да дава **strong exception guarantee**.

Изисквания:

- Ако заделянето на нова памет хвърли изключение, текущият обект да остане непроменен.
- Добавете метод `swap(basic_string& other) noexcept`.
- Реализирайте copy assignment чрез copy-and-swap идиом.

---

## Допълнителни четива:
[Should you use exceptions in C++?](https://mmomtchev.medium.com/the-true-cost-of-c-exceptions-7be7614b5d84) \
[Google Style guide on why they DON'T use exceptions](https://google.github.io/styleguide/cppguide.html#Exceptions)