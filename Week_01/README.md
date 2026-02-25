# Динамична памет, указатели към функции, изборен тип (enum), кортежи (struct)

---

## Съдържание

1. [Динамична памет](#динамична-памет)
2. [Указатели към функции](#указатели-към-функции)
3. [Побитови операции](#побитови-операции)
4. [Изборен тип (Enumerations)](#изборен-тип-enumerations)
5. [Кортежи (Structs)](#кортежи-structs)
6. [Задачи](#задачи)

---

## Динамична памет

Динамичната памет се управлява ръчно от програмиста. При заделяне на памет с `new`, ако няма налична такава, стандартно се хвърля изключение и прекъсва програмата. \
**Бележка**: `std::nothrow` е специален обект, който указва на оператора `new` да не хвърля изключение при неуспех, а вместо това да връща `nullptr`. Остарял е като практика и се среща предимно в legacy код, но ще се използва докато не вземем изключения. \
Следния код ще доведе до изчерпване на паметта и ще покаже как `std::nothrow` предотвратява изключението:

```cpp
#include <iostream>
#include <new>

int main() {
    int** allocations = new int*[100000]; // array of pointers to track allocations
    int count = 0;

    while (true) {
        allocations[count] = new(std::nothrow) int[1'000'000'000];
        if (allocations[count] == nullptr) {
            std::cout << "Failed to allocate memory for index " << count << std::endl;
            break;
        }
        count++;
    }

    // Cleanup
    for (int i = 0; i < count; i++) {
        delete[] allocations[i];
    }
    delete[] allocations;

    return 0;
}
```

## Указатели към функции

Указателите към функции позволяват предаването на функции като аргументи. За по-четимо представяне ще създадем псевдоним на типа указател към функция. В модерен C++ се използва ключовата дума `using`, която създава псевдоним (alias, да не се бърка с reference '&', което също на места се среща като псевдоним на български) на тип (по-четим вариант на `typedef`).

```cpp
typedef int(*Operation)(int, int);
// Equivalent
using Operation = int(*)(int, int);

int sum(int a, int b) { return a + b; }

int executing(int a, int b, Operation op) {
    return op(a, b);
}

int main() {
    int result = executing(5, 3, sum);
    std::cout << "Result: " << result << std::endl;
}
```

## Побитови операции

### Основни оператори
*   `&` (AND): 1 ако и двата бита са 1.
*   `|` (OR): 1 ако поне единият бит е 1.
*   `^` (XOR): 1 ако битовете са различни.
*   `~` (NOT): Обръща всички битове (0->1, 1->0).
*   `<<` (Left Shift): Измества битовете наляво (умножение по 2).
*   `>>` (Right Shift): Измества битовете надясно (делене на 2).

### Таблица на истинност за AND, OR, XOR

| A | B | A & B | A \| B | A ^ B |
|:---:|:---:|:-------:|:-------:|:-------:|
| 0 | 0 |   0   |   0   |   0   |
| 0 | 1 |   0   |   1   |   1   |
| 1 | 0 |   0   |   1   |   1   |
| 1 | 1 |   1   |   1   |   0   |


### Някои полезни трикове с побитови операции
*   **Проверка за четност:** `n & 1` (0 = четно, 1 = нечетно).
*   **Създаване на маска за първите k бита:** `(1 << k) - 1`
*   **Проверка за степен на двойката:** `(n & (n - 1)) == 0`
*   **Вдигане на бит (Set):** `mask |= (1 << k);`
*   **Изчистване на бит (Clear):** `mask &= ~(1 << k);`
*   **Обръщане на бит (Toggle):** `mask ^= (1 << k);`

#### Често като работим с побитови операции и искаме да сме сигурни в размера на данните, които ползваме, използваме типовете от `<cstdint>` (напр. `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`), които гарантират точен брой битове.

### `std::bitset`
**Важно**: Класът може да го ползвате при тестване сами. При практически задачи **НЯМА** да можете да го използвате. [За повече информация за операциите](https://en.cppreference.com/w/cpp/utility/bitset.html).
```cpp
#include <bitset>
std::bitset<8> bits(5);
std::bitset<8> mask(0b00000101);
std::cout << "Output:\n" << bits << std::endl;
```
```bash
Output:
00000101
```

### Little Endian vs Big Endian
Подредбата на байтовете в паметта зависи от архитектурата (Endianness).
*   **Little Endian:** Най-младшият байт е на най-ниския адрес (Intel/AMD x86).
*   **Big Endian:** Най-старшият байт е на най-ниския адрес (мрежови протоколи).

![Endianness](https://yoginsavani.com/wp-content/uploads/2023/05/Big-endian-and-Little-endian.png)

```cpp
// Проверка на endianness
int n = 1;
if(*(char*)&n == 1) std::cout << "Little Endian";
else std::cout << "Big Endian";
```

## Изборен тип (Enumerations)

### Разположение в паметта
`enum` е целочислен тип. Размерът му зависи от компилатора и от това дали е зададен конкретен подлежащ тип, но по подразбиране често е колкото `int` (4 байта).

### Видове Enums

1.  **Unscoped Enum:** Членовете са видими в глобалния обхват, ключовата дума е `enum`.
2.  **Scoped Enum:** Членовете са скрити вътре в enum-а и не се преобразуват автоматично към `int`. Ключовата дума е `enum class` или `enum struct` (еквивалентни). Занапред, използвайте `enum class`.

```cpp
// Unscoped
enum Color { 
    Red, 
    Green, 
    Blue 
}; // <- Make notice of the semicolon

enum Car {
    Red, // Conflict with Color::Red
    BMW,
    Audi
};

// Scoped
enum class TrafficLight { 
    Red, // No conflict with Color::Red or Car::Red 
    Yellow, 
    Green 
};
```

```cpp
// We declare an enum variable like
Color c = Red; // Unscoped, no need for Color::Red
TrafficLight t = TrafficLight::Red; // Scoped, must use scope operator '::'

// Also for some reason you might see such thing
enum class Animal {
    Omnivore,
    Herbivore,
    Carnivore
} a = Animal::Omnivore;
```

### Стойности по подразбиране

```cpp
enum class Color { Red, Green, Blue }; // Red=0, Green=1, Blue=2

// Custom values
enum class Color { Red = 1, Green = 2, Blue = 4, Yellow };
// Yellow would be 5, because it always takes the previous value + 1 if not specified
```

### Специфициране на типа
Можем да оптимизираме паметта, като укажем по-малък тип (напр. `char` – 1 байт).

```cpp
enum class SmallEnum : char { A, B, C }; // Заема 1 байт
```

При `enum class` няма автоматично преобразуване към `int`. Трябва да използваме `static_cast`.

```cpp
TrafficLight light = TrafficLight::Red;
int value = static_cast<int>(light); // 0

// The other way around
int input = 1;
TrafficLight t = static_cast<TrafficLight>(input); // TrafficLight::Yellow
```

### Фабрика за Enums
Тъй като не можем директно да създадем enum от число (напр. от потребителски вход), ни трябва функция-фабрика. Този шаблон за проектиране е полезна практика, но не е стандарт от самия език

```cpp
Color createColor(const char* const name) {
    if (strcmp(name, "Red") == 0) return Color::Red;
    else if (strcmp(name, "Green") == 0) return Color::Green;
    else if (strcmp(name, "Blue") == 0) return Color::Blue;
    throw std::invalid_argument("Unknown color");
}
```

## Кортежи (Structs)

Кортежите (структури) са взети от C, където са били използвани за създаване на собствени типове. Те групират данни и могат да съдържат функции, които ще нариаме методи (в C не могат).

```cpp
struct Point {
    // Data members
    double x = 0.0;
    double y = 0.0;

    // Methods
    void print() {
        std::cout << "(" << x << ", " << y << ")\n";
    }

    void move(double dx, double dy) {
        x += dx;
        y += dy;
    }
};

int main() {
    Point p1 = { 1.5, 2.0 }; // Initialization
    // Alternatively initialize each member separately
    // Point p1;
    // p1.x = 1.5;
    // p1.y = 2.0;

    print() // Error: 'print' was not declared in this scope
    p1.move(0.5, 1.0);       // Method call
    p1.print();
}
```

## Задачи

### Задача: Unix File System Simulator (unix_fs.cpp)

Вашата задача е да имплементирате симулатор на Unix файлова система от тему с права за достъп, използвайки побитови операции, enum класове и структури.

#### Какво трябва да направите:

1. **Настройка на Permission enum:**
   - Задайте подходящи целочислени стойности на всяко разрешение (READ, WRITE, TRUNC, APP, EXEC)
   - Стойностите трябва да могат да се използват като битови маски
   - Използвайте подходящ тип данни за максимална ефективност на паметта

2. **Имплементиране на помощни функции:**
   - `enumToString()` - функция-фабрика, преобразува Permission enum към текстово представяне
   - `grantPermission()` - добавя право за достъп към файл (използвайте побитови операции)
   - `revokePermission()` - премахва право за достъп от файл
   - `hasPermission()` - проверява дали файлът има определено право
   - **Внимавайте как работите с типовете! Не забравяйте как се преобразува enum към друг тип**

3. **Допълване на File структурата:**
   - Определете подходящия тип за `permissions` полето
   - Използвайте най-ефективния тип данни за съхранение на правата
   - Имплементирайте `info()` метода - трябва да извежда името на файла и всички негови права
   - Завършете `execute()` метода - трябва да проверява за EXEC право преди да извиква командата

4. **Имплементиране на файлови операции:**
   - `read()` - проверява за READ право и извежда съдържанието (ако няма съдържание, извежда "empty")
   - `write()` - проверява за WRITE право и добавя текст в началото на съдържанието. Ако файлът вече съдържа текст, новият текст се записва върху стария
   - `truncate()` - проверява за TRUNC право и заменя цялото съдържание с новия текст (трие старото и записва новото)
   - `append()` - проверява за APP право и добавя новия текст в края на съдържанието

#### Цялостно:
- Използвайте побитови операции (&, |, ^) за управление на правата
- Внимавайте с управлението на паметта (new/delete)
- Всички операции трябва да проверяват за съответните права преди да се изпълнят
- Не използвайте STL контейнери (string, vector и др.)
- Оптимизирайте използването на памет
