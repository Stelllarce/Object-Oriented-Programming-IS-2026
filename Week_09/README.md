# Предефиниране на оператори – основни концепции. Унарни оператори. Бинарни оператори.

## Съдържание

1. [Основни концепции](#основни-концепции)
2. [Правила и ограничения](#правила-и-ограничения)
3. [Унарни оператори](#унарни-оператори)
4. [Бинарни оператори](#бинарни-оператори)
5. [Оператори за сравнение](#оператори-за-сравнение)
6. [Операторът за индексиране `[]`](#операторът-за-индексиране-)
7. [Операторът за извикване `()`](#операторът-за-извикване-)
8. [Оператори за вход и изход](#оператори-за-вход-и-изход)
9. [Оператори за присвояване](#оператори-за-присвояване)

---

## Основни концепции

**Предефинирането на оператори** позволява да дефинирате собствено поведение на вградените оператори (`+`, `-`, `*`, `==`, `<<` и др.) за потребителски дефинирани типове. Целта е да направите кода по-четим и интуитивен – `a + b` вместо `a.add(b)`.

Операторите в C++ са всъщност функции с особен синтаксис. Когато пишете `a + b`, компилаторът търси:

```cpp
// Member function form
a.operator+(b);

// Free function form (non-member)
operator+(a, b);
```

### Пример: без и със предефиниране

```cpp
class Vector2D {
public:
    float x, y;
    Vector2D(float x, float y) : x(x), y(y) {}
};

// Without operator overloading – verbose and unnatural
Vector2D add(const Vector2D& a, const Vector2D& b) {
    return Vector2D(a.x + b.x, a.y + b.y);
}

Vector2D a(1.0f, 2.0f);
Vector2D b(3.0f, 4.0f);
Vector2D c = add(a, b);   // works, but cumbersome

// With operator overloading – natural mathematical notation
Vector2D operator+(const Vector2D& a, const Vector2D& b) {
    return Vector2D(a.x + b.x, a.y + b.y);
}

Vector2D c = a + b;        // reads like mathematics
```

### Член-функция срещу свободна функция

Предефинирането може да стане по два начина. Изборът зависи от симетрията на операнда:

```cpp
class Fraction {
    int num, den;
public:
    Fraction(int n, int d) : num(n), den(d) {}

    // Member function: left operand is always the current object (*this)
    Fraction operator+(const Fraction& rhs) const {
        return Fraction(num * rhs.den + rhs.num * den, den * rhs.den);
    }
};

// Free function: allows symmetry between operands (e.g. int + Fraction)
Fraction operator+(int lhs, const Fraction& rhs) {
    return Fraction(lhs * rhs.den + rhs.num, rhs.den);  // won't compile without friend or accessors
}
```

Когато свободната функция се нуждае от достъп до частни членове, се декларира като `friend`:

```cpp
class Fraction {
    int num, den;
public:
    Fraction(int n, int d) : num(n), den(d) {}

    // Declare the free function as a friend to grant access to private members
    friend Fraction operator+(int lhs, const Fraction& rhs);
};

Fraction operator+(int lhs, const Fraction& rhs) {
    return Fraction(lhs * rhs.den + rhs.num, rhs.den);
}
```

---

## Правила и ограничения

Не всичко е позволено при предефинирането на оператори. C++ налага следните ограничения:

**Какво НЕ може да се предефинира:**

| Оператор | Причина |
|:---|:---|
| `::` (scope resolution) | Фундаментален синтаксис на езика |
| `.` (member access) | Двусмислие с указателя към член |
| `.*` (member pointer access) | Аналогично |
| `?:` (ternary) | Изисква специална семантика на изчисление |
| `sizeof`, `typeid`, `alignof` | Работят на ниво тип, не стойност |

**Правила, които трябва да се спазват:**

1. Поне един операнд трябва да е от потребителски дефиниран тип (не можете да предефинирате `int + int`).

2. Оператори трябва да запазват своята арност - бинарните оператори остават бинарни, унарните - унарни. Например логичекият оператор `&&` винаги трябва да приема два операнда. Не можете да дефинирате `operator&&` с един параметър.

3. Не може да се променя приоритета или асоциативността на операторите. Например `*` винаги ще има по-висок приоритет от `+`, независимо от вашата дефиниция.

4. Следните оператори са задължително член-функции: `operator=`, `operator[]`, `operator()`, `operator->`.

5. Не можете да създавате нови оператори - например не можете да дефинирате `operator**` за експоненциране както в някои други езици.

> **Добра практика:** Предефинирайте оператор само когато значението му е очевидно и интуитивно. `Matrix * Matrix` е естествено. `File * File` – не.

---

## Унарни оператори

Унарните оператори работят с **един операнд** – обикновено текущия обект. Те се дефинират като член-функции без параметри (или като свободни функции с един параметър).

### Оператор за отрицание `-`

```cpp
class Vector2D {
public:
    float x, y;
    Vector2D(float x, float y) : x(x), y(y) {}

    // Unary minus: returns a new vector pointing in the opposite direction
    Vector2D operator-() const {
        return Vector2D(-x, -y);
    }
};

Vector2D v(3.0f, -1.0f);
Vector2D neg = -v;           // neg.x == -3.0f, neg.y == 1.0f
```

### Оператор за логическо отрицание `!`

```cpp
class Buffer {
    char* data;
    size_t sz;
public:
    Buffer() : data(nullptr), sz(0) {}

    // Returns true if the buffer is empty (no data allocated)
    bool operator!() const {
        return data == nullptr || sz == 0;
    }
};

Buffer buf;
if (!buf) {
    // buffer is empty – safe to check before use
}
```

### Инкремент и декремент `++` / `--`

Тук C++ прави **разлика между префиксна и постфиксна** форма. Постфиксната получава фиктивен `int` параметър – само за да различи сигнатурите:

```cpp
class Counter {
    int value;
public:
    Counter(int v = 0) : value(v) {}
    int get() const { return value; }

    // Prefix ++counter: increment, then return the new value (reference)
    Counter& operator++() {
        ++value;
        return *this;
    }

    // Postfix counter++: save old value, increment, return the old value (by value)
    Counter operator++(int) {
        Counter old = *this;   // copy before increment
        ++value;
        return old;            // return copy of old state
    }

    // Prefix --counter
    Counter& operator--() {
        --value;
        return *this;
    }

    // Postfix counter--
    Counter operator--(int) {
        Counter old = *this;
        --value;
        return old;
    }
};

Counter c(5);
Counter a = ++c;   // c becomes 6, a is 6
Counter b = c++;   // b is 6, c becomes 7
```

> **Забележка:** Префиксната форма е по-ефективна – не прави копие. Предпочитайте `++i` пред `i++` когато старата стойност не е нужна.
> Практически пример:
> В stl съществуват така наречените итератори - специални обекти, които служат за обхождане на колекции и пазят вътрешно състояние (напр. текущ индекс). Те обикновено дефинират `operator++` за придвижване към следващия елемент. В този контекст префиксната форма е по-естествена и по-ефективна, за да не копираме итератора всеки път:
> ```cpp
> for (Counter i(0); i.get() < 10; ++i) {
>     // ... do something ...
> }
> ```

### Оператор за разадресиране `*` и достъп до член `->`

Типично за **умни указатели** и итератори:

```cpp
template<typename T>
class ScopedPtr {
    T* ptr;
public:
    explicit ScopedPtr(T* p) : ptr(p) {}
    ~ScopedPtr() { delete ptr; }

    // Dereference: return a reference to the pointed-to object
    T& operator*() const { return *ptr; }

    // Arrow: allow member access through the smart pointer
    T* operator->() const { return ptr; }
};

struct Point { float x, y; };

ScopedPtr<Point> sp(new Point{1.0f, 2.0f});
sp->x = 5.0f;     // calls operator->(), then accesses x
(*sp).y = 3.0f;   // calls operator*(), then accesses y
```

### Задача

Към класа `my_std::basic_string<T>` добавете:

- `bool operator!() const` – връща `true` ако низът е празен (дължина 0 или `nullptr` буфер).
- `basic_string& operator++()` – **префиксна** форма: конвертира всеки символ в главен (uppercase) и връща `*this`.
- `basic_string operator++(int)` – **постфиксна** форма: запазва копие на текущото съдържание, конвертира оригинала в главни символи (uppercase) и връща старото копие.

---

## Бинарни оператори

Бинарните оператори работят с **два операнда**. Съществуват два типа: **аритметични** (връщат нова стойност) и **аритметично-присвояващи** (модифицират левия операнд и връщат `*this`).

### Основен модел: `+=` и `+`

Препоръчителният подход е да дефинирате `+=` като член-функция (тя модифицира обекта директно), а след това да изведете `+` от нея като свободна функция:

```cpp
class BigInt {
    int data[64];
    size_t used;
public:
    BigInt() : used(0) { /* initialize */ }

    // Compound assignment: modify in place, return reference to self
    BigInt& operator+=(const BigInt& rhs) {
        // ... addition logic ...
        return *this;
    }

    // Binary +: defined in terms of +=, avoids code duplication
    // Takes left operand by value so we can modify it freely
    friend BigInt operator+(BigInt lhs, const BigInt& rhs) {
        lhs += rhs;
        return lhs;
    }
};
```

> **Принцип:** Дефинирайте `op=` като член, после изведете `op` от него. Двете операции остават синхронизирани автоматично.

### Аритметични оператори за `Vector2D`

```cpp
class Vector2D {
public:
    float x, y;
    Vector2D(float x = 0, float y = 0) : x(x), y(y) {}

    Vector2D& operator+=(const Vector2D& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2D& operator-=(const Vector2D& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    // Scale by scalar: member because left operand is always Vector2D
    Vector2D& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    friend Vector2D operator+(Vector2D lhs, const Vector2D& rhs) { return lhs += rhs; }
    friend Vector2D operator-(Vector2D lhs, const Vector2D& rhs) { return lhs -= rhs; }

    // Both orderings: Vector2D * float  and  float * Vector2D
    friend Vector2D operator*(Vector2D v, float s) { return v *= s; }
    friend Vector2D operator*(float s, Vector2D v) { return v *= s; }
};

Vector2D a(1.0f, 2.0f), b(3.0f, 4.0f);
Vector2D c = a + b;          // (4, 6)
Vector2D d = 2.0f * a;       // (2, 4)  – uses float * Vector2D overload
```

### Задача

Към класа `my_std::basic_string<T>` добавете:

- `basic_string& operator+=(const basic_string& rhs)` – добавя съдържанието на `rhs` в края на текущия низ (конкатенация). Ако буферът не е достатъчно голям, заделете нов.
- `basic_string& operator+=(T ch)` – добавя единичен символ в края на низа.
- `friend basic_string operator+(basic_string lhs, const basic_string& rhs)` – връща нов низ, конкатенация на двата, дефиниран чрез `+=`.
- `friend basic_string operator+(basic_string lhs, T ch)` – добавя символ към копие на `lhs`.

---

## Оператори за сравнение

Операторите за сравнение трябва да са **константни** (не модифицират обекта) и обикновено се дефинират като свободни функции за симетрия.

### Модел: `==` и `!=`

Дефинирайте `==`, после изведете `!=` от него:

```cpp
class Date {
    int year, month, day;
public:
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    friend bool operator==(const Date& lhs, const Date& rhs) {
        return lhs.year == rhs.year
            && lhs.month == rhs.month
            && lhs.day == rhs.day;
    }

    // Define != in terms of ==
    friend bool operator!=(const Date& lhs, const Date& rhs) {
        return !(lhs == rhs);
    }
};
```

### Пълен набор оператори за наредба

За типове с естествена наредба (числа, дати, низове) дефинирайте `<`, после изведете останалите:

```cpp
class Date {
    int year, month, day;
public:
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    friend bool operator<(const Date& lhs, const Date& rhs) {
        if (lhs.year  != rhs.year)  return lhs.year  < rhs.year;
        if (lhs.month != rhs.month) return lhs.month < rhs.month;
        return lhs.day < rhs.day;
    }

    // Derive all other comparisons from operator< and operator==
    friend bool operator> (const Date& lhs, const Date& rhs) { return rhs < lhs; }
    friend bool operator<=(const Date& lhs, const Date& rhs) { return !(rhs < lhs); }
    friend bool operator>=(const Date& lhs, const Date& rhs) { return !(lhs < rhs); }
    friend bool operator==(const Date& lhs, const Date& rhs) {
        return !(lhs < rhs) && !(rhs < lhs);
    }
    friend bool operator!=(const Date& lhs, const Date& rhs) { return !(lhs == rhs); }
};

Date d1(2024, 1, 15), d2(2024, 3, 20);
bool earlier = d1 < d2;    // true
bool same    = d1 == d2;   // false
```

> **Принцип:** Дефинирайте само `<` и `==`. Останалите четири оператора следват механично.

### Задача

Към класа `my_std::basic_string<T>` добавете пълен набор оператори за сравнение:

- `friend bool operator==(const basic_string& lhs, const basic_string& rhs)` – лексикографско равенство (сравнявайте символ по символ; различна дължина → неравни).
- `friend bool operator!=(const basic_string& lhs, const basic_string& rhs)` – дефинирайте чрез `==`.
- `friend bool operator<(const basic_string& lhs, const basic_string& rhs)` – лексикографска наредба.
- `friend bool operator>`, `operator<=`, `operator>=` – дефинирайте чрез `<`.

---

## Операторът за индексиране `[]`

Операторът `[]` позволява достъп до елементи по индекс. Трябва да се дефинира като член-функция и **задължително** в две версии – константна и неконстантна:

```cpp
class CharBuffer {
    char* data;
    size_t sz;
public:
    CharBuffer(size_t n) : sz(n), data(new char[n]()) {}
    ~CharBuffer() { delete[] data; }

    // Non-const version: allows modification of elements
    char& operator[](size_t index) {
        return data[index];
        // Alternatively, if data does not have operator[] defined we can do:
        // return *(data + index);
    }

    // Const version: called on const objects, does not allow modification
    const char& operator[](size_t index) const {
        return data[index];
    }

    size_t size() const { return sz; }
};

CharBuffer buf(10);
buf[0] = 'H';           // calls non-const operator[]
buf[1] = 'i';

const CharBuffer& ref = buf;
char c = ref[0];        // calls const operator[] – read only
// ref[0] = 'X';        // ERROR – const version returns const reference
```

> **Важно:** Без константната версия обектите, предадени като `const T&`, не могат да се индексират – компилаторът ще откаже да компилира.

### Задача

Към класа `my_std::basic_string<T>` добавете:

- `T& operator[](size_t index)` – неконстантна версия, позволяваща модификация на символа на позиция `index`.
- `const T& operator[](size_t index) const` – константна версия за четене.

---

## Операторът за извикване `()`

Операторът `()` прави обект **извикваем като функция** (функционален обект, или *functor*). Той е задължително член-функция и може да е с произволен брой параметри.

```cpp
class Multiplier {
    int factor;
public:
    explicit Multiplier(int f) : factor(f) {}

    // operator(): makes the object callable like a function
    int operator()(int x) const {
        return x * factor;
    }
};

Multiplier triple(3);
int result = triple(7);    // calls operator()(7), returns 21
int result2 = triple(10);  // returns 30

// Useful as a stateful callback – unlike plain functions, functors can carry state
```

Функционалните обекти са особено полезни когато трябва да предадете **параметризирана логика**:

```cpp
class RangeChecker {
    int lo, hi;
public:
    RangeChecker(int lo, int hi) : lo(lo), hi(hi) {}

    // Returns true if value falls within [lo, hi]
    bool operator()(int value) const {
        return value >= lo && value <= hi;
    }
};

RangeChecker inRange(1, 100);
bool valid = inRange(42);     // true
bool invalid = inRange(200);  // false
```

---

## Оператори за вход и изход

Операторите `<<` и `>>` за потоци **не могат** да са член-функции на вашия клас (защото левият операнд е `std::ostream`/`std::istream`, а не вашия обект). Дефинират се като свободни функции – обикновено приятелски:

```cpp
#include <iostream>

class Complex {
    double re, im;
public:
    Complex(double r = 0, double i = 0) : re(r), im(i) {}

    // Declare as friend to access private members
    friend std::ostream& operator<<(std::ostream& os, const Complex& c);
    friend std::istream& operator>>(std::istream& is, Complex& c);
};

// Output operator: must return the stream to allow chaining (cout << a << b)
std::ostream& operator<<(std::ostream& os, const Complex& c) {
    os << c.re;
    if (c.im >= 0) os << '+';
    os << c.im << 'i';
    return os;    // return stream to support chaining
}

// Input operator: reads two doubles from the stream
std::istream& operator>>(std::istream& is, Complex& c) {
    is >> c.re >> c.im;
    return is;
}

Complex a(3.0, -2.0), b(1.0, 4.0);
std::cout << a << " and " << b << '\n';   // chaining works because we return os
```

> **Правило:** Операторите `<<` и `>>` винаги трябва да връщат референция към потока, за да поддържат верижно извикване.

### Въщане назад към шаблоните

Ако имаме шаблонен клас, понеже операторите за вход/изход са свободни функции, те също трябва да са шаблонни, но параметъра за типа трябва да е упоменат специално, а не да се взима този на шаблонния клас:

```cpp
template<typename T>
class Foo {
    T value;
public:
    Foo(T v) : value(v) {}
    // Friend declarations for stream operators
    friend std::ostream& operator<<(std::ostream& os, const Foo<T>& foo) {
        os << foo.value;
        return os;
    }
    friend std::istream& operator>>(std::istream& is, Foo<T>& foo) {
        is >> foo.value;
        return is;
    }
};
```
В този случай операторите са дефинирани вътре в класа, но са шаблонни и могат да работят с всеки тип `T`. Ако ги дефинираме извън класа, трябва да използваме шаблонна декларация:

```cpp
template<typename T>
class Bar {
    T value;
public:
    Bar(T v) : value(v) {}
    // Friend declarations for stream operators
    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Bar<U>& bar);
    template<typename U>
    friend std::istream& operator>>(std::istream& is, Bar<U>& bar);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Bar<T>& bar) {
    os << bar.value;
    return os;
}

template<typename T>
std::istream& operator>>(std::istream& is, Bar<T>& bar) {
    is >> bar.value;
    return is;
}
```

### Задача

Към класа `my_std::basic_string<T>` (специализиран за `char`) добавете:

- `friend std::ostream& operator<<(std::ostream& os, const basic_string<char>& str)` – извежда символите на низа в потока един по един.
- `friend std::istream& operator>>(std::istream& is, basic_string<char>& str)` – чете символи от потока до първия whitespace и ги записва в низа (презаписва старото съдържание).

---

## Оператори за присвояване

Операторите за присвояване **задължително** са член-функции. Стандартното `operator=` е разгледано в темата за Голямата шестица. Тук разглеждаме съставните оператори за присвояване (`+=`, `-=`, `*=` и др.) като самостоятелна концепция.

### Идиом: проверка за самоприсвояване

При копиращото присвояване е важно да се провери дали обектът се присвоява сам на себе си:

```cpp
class Matrix {
    float* data;
    size_t rows, cols;
public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(new float[r * c]()) {}
    ~Matrix() { delete[] data; }

    Matrix& operator=(const Matrix& rhs) {
        if (this == &rhs) return *this;   // self-assignment guard

        if (rows * cols != rhs.rows * rhs.cols) {
            delete[] data;
            data = new float[rhs.rows * rhs.cols];
        }

        rows = rhs.rows;
        cols = rhs.cols;

        for (size_t i = 0; i < rows * cols; ++i)
            data[i] = rhs.data[i];

        return *this;
    }

    // Compound assignment: scale every element by a scalar
    Matrix& operator*=(float scalar) {
        for (size_t i = 0; i < rows * cols; ++i)
            data[i] *= scalar;
        return *this;
    }
};
```

### Задача

Към класа `my_std::basic_string<T>` добавете:

- `basic_string& operator=(const basic_string& rhs)` – **копиращо присвояване** с проверка за самоприсвояване. Освободете стария буфер, заделете нов с размер `rhs.length() + 1` и копирайте символите.
- `basic_string& operator=(basic_string&& rhs) noexcept` – **преместващо присвояване**: вземете ресурсите на `rhs` и оставете `rhs` в празно, валидно състояние.
- `basic_string& operator=(const T* cstr)` – присвояване от C-низ (указател към нулево-терминиран масив от `T`).