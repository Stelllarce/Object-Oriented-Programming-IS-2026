# Шаблони (templates). Идея, особености, използване. Шаблони на функции.

---

## Съдържание

1. [Преговор: претоварване на функции (function overloading)](#преговор-претоварване-на-функции-function-overloading)
2. [Каква е идеята на templates](#каква-е-идеята-на-templates)
3. [Синтаксис на function template](#синтаксис-на-function-template)
4. [Извеждане на template аргументи](#извеждане-на-template-аргументи)
5. [Явно задаване на template аргументи](#явно-задаване-на-template-аргументи)
6. [Шаблони с повече от един тип](#шаблони-с-повече-от-един-тип)
7. [Специализация на шаблони](#специализация-на-шаблони)
8. [Шаблонни параметри по подразбиране](#шаблонни-параметри-по-подразбиране)
9. [Не-типови шаблонни параметри](#не-типови-шаблонни-параметри)
10. [Вариадични шаблонни параметри](#вариадични-шаблонни-параметри)
11. [Защо шаблоните не работят с разделна компилация](#защо-шаблоните-не-работят-с-разделна-компилация)
12. [Особености и чести грешки](#особености-и-чести-грешки)
13. [Кога да използваме шаблони](#кога-да-използваме-шаблони)
14. [Обобщение](#обобщение)

---

## Преговор: претоварване на функции (function overloading)

Преди да разгледаме шаблоните, е полезно да припомним **function overloading** — механизъм, с който вече сме запознати.

Overloading ни позволява да дефинираме няколко функции с едно и също име, но с различни параметри:

```cpp
int maxValue(int a, int b) {
    return (a > b) ? a : b;
}

double maxValue(double a, double b) {
    return (a > b) ? a : b;
}

char maxValue(char a, char b) {
    return (a > b) ? a : b;
}
```

Компилаторът избира правилната версия въз основа на типовете аргументи при извикването. Това е удобно, но забелязваме ясен проблем: **трите реализации са идентични по логика**. Ако алгоритъмът трябва да се промени, трябва да го поправяме на три места.

Точно тук влизат в употреба **шаблоните**.

---

## Каква е идеята на шаблоните

Шаблоните са рецепта/схема за създаване на функции, а не конкретни такива. При извикване на шаблонна функция се създава **инстанция** на конкретната функция за нужния тип. Всичко това се случва по време на **компилация**.

Това ни позволява да напишем **един общ алгоритъм**, който да работи за много типове. Вместо трите функции по-горе можем да напишем:

```cpp
template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

При инстанциране:

```cpp
int main() {
    std::cout << maxValue(3, 5) << "\n";      // T is deduced as int
    std::cout << maxValue(1.2, 3.4) << "\n";  // T is deduced as double
    std::cout << maxValue('b', 'k') << "\n";  // T is deduced as char
    return 0;
}
```

Така:

- избягваме дублиране на код
- логиката стои на едно място
- компилаторът създава нужните конкретни версии автоматично

> **Много важно за изпита:** Шаблонът не премахва нуждата операциите да са смислени. Ако използваме `a > b`, типът `T` трябва да поддържа `operator>`.

---

## Синтаксис на function template

Основната форма е:

```cpp
template <typename T>
T functionName(T value) {
    return value;
}
```

- `template <typename T>` казва, че дефинираме шаблон
- `T` е **параметър на шаблона** — placeholder за конкретен тип
- при инстанциране `T` се замества с конкретен тип

`typename` и `class` тук са еквивалентни:

```cpp
template <typename T>
T identityA(T value) {
    return value;
}

template <class T>
T identityB(T value) {
    return value;
}
```

[Относно кога има разлика между двете](https://stackoverflow.com/questions/2023977/what-is-the-difference-between-typename-and-class-template-parameters)

### Прост пример: размяна на две стойности

```cpp
#include <iostream>

template <typename T>
void swapValues(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

int main() {
    int x = 10;
    int y = 20;
    swapValues(x, y);

    std::cout << x << " " << y << "\n";
    return 0;
}
```

---

## Извеждане на template аргументи

В повечето случаи не е нужно да пишем типа ръчно — компилаторът го **дедуцира** от аргументите на функцията:

```cpp
#include <iostream>

template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}

int main() {
    std::cout << maxValue(4, 9) << "\n";        // T becomes int
    std::cout << maxValue(1.2, 3.4) << "\n";    // T becomes double
    std::cout << maxValue('b', 'k') << "\n";    // T becomes char
    return 0;
}
```

### Кога дедукцията се проваля?

Ако аргументите са от различни типове, компилаторът не може да реши какъв да бъде `T`:

```cpp
template <typename T>
T maxValue(const T& a, const T& b) { // a and b are promised to be the same type T
    return (a > b) ? a : b;
}

int main() {
    maxValue(3, 4.5); // Error: conflicting deductions for T
    return 0;
}
```

Едното решение е да използваме два отделни типа:

```cpp
template <typename T, typename U>
T maxValue(const T& a, const U& b) {
    return (a > b) ? a : b;
}

int main() {
    maxValue(3, 4.5); // Now works: T = int, U = double
    return 0;
}
```

---

## Явно задаване на template аргументи

Понякога искаме да зададем типа изрично, вместо да разчитаме на дедукция:

```cpp
#include <iostream>

template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}

int main() {
    // T is fixed as double, so 3 is implicitly converted
    std::cout << maxValue<double>(3, 4.5) << "\n";
    std::cout << maxValue<int>(7, 2) << "\n";
    return 0;
}
```

При `maxValue<double>(3, 4.5)`:

- `T` е фиксиран като `double`
- стойността `3` се преобразува до `double`
- извиква се `maxValue<double>`

---

## Шаблони с повече от един тип

Шаблонната функция може да има повече от един параметър на шаблона:

```cpp
#include <iostream>

template <typename T, typename U>
void printPair(const T& first, const U& second) {
    std::cout << first << " " << second << "\n";
}

int main() {
    printPair(42, 3.14);
    printPair('A', "hello");
    return 0;
}
```

Тук `T` и `U` могат да бъдат различни типове. **Функцията ще работи, ако и двата типа могат да се отпечатат с `<<`.**

Още един пример:

```cpp
template <typename T, typename U>
bool areEqual(const T& a, const U& b) {
    return a == b;
}
```

**Тази функция ще работи само ако между `a` и `b` има валидно сравнение с `==`.**

---

## 🔍 Задача, подобна на такава от изпит: Какви ограничения налагат операциите?

Разгледайте следната шаблонна функция и отговорете на въпросите по-долу преди да продължите с материала:

```cpp
template <typename T, typename U, typename V, typename W>
void process(const T& a, U* b, V& c, W d) {
    if (a < *b) {
        c = c + a;
    }

    *b = *b - d;

    if (c == W{}) {
        ++d;
    }

    std::cout << c << "\n";
}
```

1. Какви операции трябва да поддържа типът `T`?
2. Какви операции трябва да поддържа типът `U`?
3. Какви операции трябва да поддържа типът `V`?
4. Какви операции трябва да поддържа типът `W`?
5. Задължително ли е `V` и `W` да са еднакви типове? Защо?
6. Задължително ли е `U` и `W` да са еднакви типове? Защо?

<details>
<summary>Отговори</summary>

**`T`** трябва да поддържа:
- `operator<` спрямо `U` (от `a < *b`) — `T` и `U` трябва да са сравними
- `operator+` с `V` или типът, върнат от `operator+`, да се присвоява на `V` (от `c = c + a`) — `V` трябва да поддържа `operator+(const V&, const T&)` и присвояване `=`

**`U`** трябва да поддържа:
- разадресиране (`*b`) — т.е. `U` трябва да е указател или тип с `operator*`
- `operator-` с `W` (от `*b = *b - d`) — резултатът трябва да може да се присвои на `*b`

**`V`** трябва да поддържа:
- `operator+` с `T` и присвояване `=` (от `c = c + a`)
- `operator==` спрямо `W` (от `c == W{}`)
- `operator<<` (от `std::cout << c`)

**`W`** трябва да поддържа:
- value-initialization `W{}` — т.е. да може да се конструира по подразбиране
- `operator++` (от `++d`)
- `operator-` с друг `W` (от `*b = *b - d`, ако `U` и `W` са свързани типове)

**`V` и `W`** не трябва да са еднакви типове, но `V` трябва да може да се сравнява с `W{}`.

**`U` и `W`** не трябва да са еднакви, но `*b - d` трябва да е валидна операция между `U` и `W`.

</details>

---

## Специализация на шаблони

Понякога общата версия на шаблона не е достатъчна за конкретен тип. Тогава можем да направим **специализация**:

```cpp
#include <cstring>

template <typename T>
bool areEqual(const T& a, const T& b) {
    return a == b;
}

// For const char*, compare by content, not by pointer address
template <>
bool areEqual<const char*>(const char* const& a, const char* const& b) {
    return std::strcmp(a, b) == 0;
}
```

```cpp
#include <iostream>

int main() {
    std::cout << areEqual(10, 10) << "\n";
    std::cout << areEqual("abc", "abc") << "\n"; // Uses the specialization
    return 0;
}
```

При второто извикване се използва специализираната версия за `const char*`.

---

## Шаблонни параметри по подразбиране

Шаблонен параметър може да има стойност по подразбиране:

```cpp
#include <iostream>

template <typename T = int>
T makeValue() {
    return T{};
}

int main() {
    int a = makeValue<>();         // Uses the default: int
    double b = makeValue<double>();

    std::cout << a << " " << b << "\n";
    return 0;
}
```

Това е удобно, когато имаме „естествен" тип по подразбиране, но искаме възможност да го сменим.

---

## Не-типови шаблонни параметри

Шаблонният параметър не е задължително да е тип. Може да е и **стойност**, известна по време на компилация:

```cpp
#include <iostream>

template <int N>
int multiplyBy(int value) {
    // N is a compile-time constant
    return value * N;
}

int main() {
    std::cout << multiplyBy<3>(7) << "\n";
    std::cout << multiplyBy<10>(2) << "\n";
    return 0;
}
```

Полезен случай е автоматичното извличане на размер на масив:

```cpp
#include <cstddef>
#include <iostream>

template <typename T, std::size_t N>
void printArray(const T (&arr)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
}
```

Тук `N` се извлича автоматично от размера на масива при инстанцирането.

---

## Вариадични шаблонни параметри

Понякога искаме шаблон, който приема произволен брой аргументи. Това става с **parameter pack** (`typename... Args`):

```cpp
#include <iostream>

template <typename T>
T sumAll(T value) {
    return value;
}

template <typename T, typename... Args>
auto sumAll(T value, Args... args) {
    return value + sumAll(args...);
}

template <typename T>
void printAll(const T& value) {
    std::cout << value << "\n";
}

template <typename T, typename... Args>
void printAll(const T& value, const Args&... args) {
    std::cout << value << " ";
    printAll(args...);
}

int main() {
    std::cout << "Sum (ints): " << sumAll(1, 2, 3, 4) << "\n";
    std::cout << "Sum (doubles): " << sumAll(1.5, 2.5, 3.0) << "\n";

    std::cout << "Print variadic args: ";
    printAll("Hero", 42, 3.14);

    return 0;
}
```

Вариадичните шаблони са основа на много части от стандартната библиотека (например `std::tuple`).

---

## Защо шаблоните не работят с разделна компилация

Вече знаем какво правят шаблоните — сега е важно да разберем едно ключово практическо ограничение.

При обикновените функции е нормално да имаме:

- декларация в `.hpp`
- дефиниция в `.cpp`
- използване от други `.cpp` файлове

Това работи, защото компилаторът генерира код за обикновената функция още когато види нейната дефиниция.

При **шаблоните** нещата са различни. Шаблонът сам по себе си **не е истинска функция**, а **рецепта/схема** за създаване на функции. Реален код се генерира чак когато компилаторът види използване с конкретен тип.

### Какъв е проблемът?

Ако сложим декларацията в `.hpp`, а дефиницията в `.cpp`, файлът, който използва шаблона, вижда само декларацията, но **не вижда тялото**. Точно в този момент компилаторът трябва да създаде конкретната версия, но няма достатъчно информация.

```cpp
// max.hpp
template <typename T>
T maxValue(const T& a, const T& b);
```

```cpp
// max.cpp
#include "max.hpp"

template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

```cpp
// main.cpp
#include <iostream>
#include "max.hpp"

int main() {
    std::cout << maxValue(3, 5) << "\n";
    return 0;
}
```

При компилацията на `main.cpp`:

- компилаторът вижда, че се иска `maxValue<int>`
- вижда декларацията
- **не вижда дефиницията**
- не може да генерира кода за `maxValue<int>`

При компилацията на `max.cpp`:

- компилаторът вижда дефиницията на шаблона
- но не вижда използване на `maxValue<int>`
- не генерира никаква конкретна функция

Резултатът е грешка при линкване или липсващ символ.

> **Извод:** Приемайте шаблоните като **header-only** код. Декларацията и дефиницията им стоят в `.hpp` файла.

```cpp
// max.hpp — declaration and definition together
template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

Има техники като **explicit instantiation**, при които част от шаблонния код може да бъде в `.cpp`, но това е изключение. За този курс правилото е:

### Алтернатива на това да пишете всичко в `.hpp`

### Enter `inline`

Когато извикаме обикновена функция, програмата извършва редица допълнителни операции: запазва текущото състояние, прескача към адреса на функцията, изпълнява я, след което се връща обратно. При малки функции, извиквани много пъти, тези допълнителни стъпки могат да имат нежелана цена.

Ключовата дума `inline` е **подсказка към компилатора** да замести извикването на функцията директно с нейното тяло на място в кода.  

```cpp
inline int square(int x) {
    // Body is substituted directly at the call site
    return x * x;
}

int main() {
    int result = square(5); // Compiler may treat this as: int result = 5 * 5;
    return 0;
}
```

### Важно: inline е подсказка, не команда

Компилаторът **не е задължен** да се съобрази с `inline`. Може да го игнорира, ако прецени, че функцията е твърде голяма или сложна. Обратното също е вярно — компилаторът може да inline-ва функции, на които не сме написали `inline`, ако сам прецени, че е изгодно, особено при къси функции.

### Връзка с шаблоните

Тук се появява интересна връзка с темата на документа. Шаблонните функции **имплицитно са inline** — точно защото дефиницията им трябва да е видима в header файла и ще бъде включена в множество `.cpp` файла. Компилаторът третира всяка инстанция по начин, аналогичен на `inline`.

```cpp
// No need to write inline explicitly — template functions are implicitly inline
template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

За да запазим подредбата на кода. бихме могли да оставим декларацийте в `.hpp` и да сложим дефинициите в `.inl` файл, който впоследствие включваме в `.hpp`:

**max.hpp**
```cpp
#pragma once
template <typename T>
T maxValue(const T& a, const T& b);

#include "max.inl" // Include the definitions
```

**max.inl**
```cpp
template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

## Особености и чести грешки

### Грешките се появяват при използване, не при дефиниране

Може да напишем шаблон без грешка, но проблемът да се появи чак когато го използваме с неподходящ тип:

```cpp
template <typename T>
void printTwice(const T& value) {
    std::cout << value << " " << value << "\n";
}
```

Ако `T` не поддържа `operator<<`, грешката ще се появи при инстанцирането на `printTwice<T>`.

### Понякога обикновена функция е по-добър избор

Ако функцията има смисъл само за един конкретен тип, шаблонът не носи полза:

```cpp
double calculateCircleArea(double radius) {
    return 3.14159 * radius * radius;
}
```

Това не е добра кандидатура за шаблон — логиката е естествено свързана с `double`.

### Ръчно компилиране на header файлове, покрай шаблоните

Ако имаме просто един файл със шаблонни функции, които искаме да изпълним в main, някой може да си помисли, че няма как да се компилират тези фунцкии, щом нямат асоцииран `.cpp` файл.

```cpp
// max.hpp
#pragma once
template <typename T>
T maxValue(const T& a, const T& b) {
    return (a > b) ? a : b;
}
```

```cpp
// main.cpp
#include "max.hpp"
#include <iostream>
int main() {
    std::cout << maxValue(3, 5) << "\n";
    return 0;
}
```

```bash
g++ main.cpp max.hpp -o main # Не е грешно, но е излишно, вече имаме .cpp файл. който съдържа хедъра и копилатора чрез него ще компилира всичко успешно, така че:
g++ main.cpp -o main # Това е достатъчно
```

---

## Кога да използваме шаблони на фунцкии

Използвайте function template, когато:

- алгоритъмът е един и същ за много типове
- разликата е само в типа на аргументите
- използваните операции са смислени за тези типове


Не използвайте шаблон, когато:

- функцията е логически вързана към един конкретен тип
- различните типове изискват различна логика
- обикновена overload функция е по-ясна и по-проста

### Кратко сравнение

| Подход | Кога е подходящ |
|--------|-----------------|
| Обикновена функция | Когато работим с един конкретен тип |
| Overloading | Когато имаме няколко близки, но различни реализации |
| Function template | Когато една и съща логика работи за много типове |

---

### Допълнителни четива
[От създателя на c++ за шаблони](https://www.stroustrup.com/bs_faq2.html#constraints) \
[Как ги обясняват miscrosoft](https://learn.microsoft.com/en-us/cpp/cpp/templates-cpp?view=msvc-170) \
[CppCon 2022](https://www.youtube.com/watch?v=HqsEHG0QJXU)

## Задачи

---

### Кое компилира? (без да пускате кода)

Дадена е следната шаблонна функция:
```cpp
template <typename T>
T add(const T& a, const T& b) {
    return a + b;
}
```

За всяко извикване по-долу посочете: **компилира ли се**, и ако да — какъв е типът `T`?

| # | Извикване | Компилира? | Тип T |
|---|-----------|------------|-------|
| а | `add(1, 2)` | ? | ? |
| б | `add(1.0, 2.0)` | ? | ? |
| в | `add(1, 2.0)` | ? | ? |
| г | `add<double>(1, 2.0)` | ? | ? |
| д | `add<int>(1, 2.0)` | ? | ? |
| е | `add("abc", "d")` | ? | ? |

<details>
<summary>Отговори</summary>

| # | Извикване | Компилира? | Бележка |
|---|-----------|------------|---------|
| а | `add(1, 2)` | ✓ Да | `T = int` |
| б | `add(1.0, 2.0)` | ✓ Да | `T = double` |
| в | `add(1, 2.0)` | ✗ Не | Конфликт: `T = int` срещу `T = double`. Дедукцията се проваля. |
| г | `add<double>(1, 2.0)` | ✓ Да | `T = double`, числото `1` се конвертира до `double`. |
| д | `add<int>(1, 2.0)` | ✓ Да | `T = int`, `2.0` се конвертира до `int`. Може да предизвика предупреждение. |
| е | `add("abc", "def")` | ✗ Не | `T = const char*`, но `operator+` не съществува за суров указател. Грешката се вижда чак при инстанцирането. |

> **Случай е** е капан — дедукцията минава успешно (и двата аргумента са `const char*`), но операцията `+` не е дефинирана за указатели. Грешката се вижда чак при инстанцирането.

</details>

## Задача — Регистър на метеорологични станции

Ще напишете мини-система за съхранение и обработка на данни от метеорологични станции. Данните се записват и четат от **бинарни файлове**.

---

### Дадени структури

Работете с тези две структури. **Не ги променяйте.**
```cpp
struct Station {
    int   id;
    char  name[32];
    float latitude;
    float longitude;
};

template <typename T>
struct Measurement {
    int station_id;
    int timestamp;
    T   value;
};
```

---

### Подзадачи

#### `writeToFile`

Напишете шаблонна функция:
```cpp
template <typename T, std::size_t N>
bool writeToFile(const char* filename, const T (&arr)[N]);
```

Функцията отваря файл за **бинарен запис**, записва всички `N` елемента наведнъж с едно извикване на `write()` и връща `true` при успех, `false` при грешка. Не използвайте цикъл.

---

#### `readFromFile`

Напишете шаблонна функция:
```cpp
template <typename T>
bool readFromFile(const char* filename, T* arr, std::size_t count);
```

Функцията отваря файл за **бинарно четене**, чете точно `count` записа в подадения масив и връща `true` при успех. Помислете как да проверите дали четенето е успешно.

---

#### `countRecords`

Напишете шаблонна функция:
```cpp
template <typename T>
std::size_t countRecords(const char* filename);
```

Функцията трябва да определи колко записа от тип `T` се съдържат в бинарния файл, **без да чете самите данни**. Използвайте позиционирането в потока.

> 💡 Помислете как размерът на файла в байтове се свързва с `sizeof(T)`.

---

#### `findFirst` с предикат

Напишете шаблонна функция:
```cpp
template <typename T, std::size_t N>
const T* findFirst(const T (&arr)[N], bool (*pred)(const T&));
```

Функцията обхожда масива и връща **указател** към първия елемент, за който предикатът връща `true`. Ако няма такъв — връща `nullptr`.

Напишете и **два конкретни предиката** (обикновени функции, не ламбди):
- Един за `Station`, който проверява дали ширината (`latitude`) е над `42.0f`
- Един за `Measurement<float>`, който проверява дали стойността надвишава `35.0f`

---

#### Специализация на `printRecord`

Напишете шаблонна функция `printRecord` с **обща версия** и **две пълни специализации**:
```cpp
// Обща версия — отпечатва само размера на типа в байтове
template <typename T>
void printRecord(const T& record);

// Специализация за Station
// Формат: "Станция #<id> | <name> | <lat>, <lon>"
template <>
void printRecord<Station>(const Station& s);

// Специализация за Measurement<float>
// Формат: "Измерване от станция #<id> @ t=<timestamp> | стойност: <value>"
template <>
void printRecord<Measurement<float>>(const Measurement<float>& m);
```

---

#### Интеграция — съберете всичко заедно

В `main` направете следното:

1. Декларирайте масив от **4 станции** с реални или измислени данни (поне 2 с `latitude > 42.0f`)
2. Декларирайте масив от **5 измервания** от тип `Measurement<float>` (поне 2 с `value > 35.0f`)
3. Запишете и двата масива в **два отделни бинарни файла** с `writeToFile`
4. Използвайте `countRecords`, за да проверите колко записа има в **двата файла** и отпечатайте резултата
5. Прочетете данните обратно в **нови масиви** с `readFromFile`
6. Обходете прочетените станции и намерете първата с `latitude > 42.0f` чрез `findFirst` — отпечатайте я с `printRecord`
7. Обходете прочетените измервания и намерете първото с `value > 35.0f` — отпечатайте го с `printRecord`
