# Двоични файлове. Работа при пряк достъп.

## Съдържание

1. [Какво е двоичен файл?](#1-какво-е-двоичен-файл)
2. [Текстов срещу двоичен файл](#2-текстов-срещу-двоичен-файл)
3. [Отваряне на двоичен файл](#3-отваряне-на-двоичен-файл)
4. [Писане – `write()`](#4-писане--write)
5. [Четене – `read()` и `gcount()`](#5-четене--read-и-gcount)
6. [Записване и четене на структури](#6-записване-и-четене-на-структури)
7. [Пряк достъп (Random Access)](#7-пряк-достъп-random-access)
8. [Изчисляване на брой записи](#8-изчисляване-на-брой-записи)
9. [Обобщение](#9-обобщение)
[Задачи](#задачи)

---

## 1. Какво е двоичен файл?

**Двоичният файл** съхранява данни в **точния им машинен вид** – тоест байт по байт, без никакви преобразувания.  
Не съществуват „специални" символи – всеки байт се интерпретира буквално.

Примери за двоични файлове: `.exe`, `.png`, `.mp3`, `.bin`, `.dat` \

Създавайте вашите двоични файлове за сега с разширение `.bin`.

### Как изглеждат данните?

Ако запишем числото `1000` (тип `int`, 4 байта на x86):

| Формат | Байтове на диска |
|--------|-----------------|
| Текстов | `'1'` `'0'` `'0'` `'0'` = **4 байта** |
| Двоичен | `0xE8` `0x03` `0x00` `0x00` = **4 байта** |

На пръв поглед размерът е еднакъв за малки числа, но за голям масив от числа двоичният формат е **значително по-компактен и по-бърз** за четене/писане.

## 2. Текстов срещу двоичен файл

| Критерий | Текстов файл | Двоичен файл |
|----------|-------------|-------------|
| Представяне на данните | Символи (ASCII/UTF-8) | Директни байтове от паметта |
| Четим от човек | Да | Не (без специален инструмент) |
| Преобразувания на `\n` | Да (Windows: `\n` ↔ `\r\n`) | Пренасят се директно |
| Скорост на I/O | По-бавен (парсване) | **По-бърз** (блоково копиране) |
| Размер | По-голям за числа | **По-малък** за числови данни |
| Пряк достъп до запис N | Труден (редовете са с различна дължина) | **Лесен** (фиксиран размер на запис) |
| Преносимост | Висока | Зависи от endianness и изравняване |

Ето пример как двоичния файл е по-компактен от текстовия:

```
Искаме да запишем числото 1234567890 в текстов и двоичен файл.
Текстов файл
"1234567890" (10 символа) → 10 байта (всеки символ е записан като ASCII код)

Двоичен файл
0xD2 0x02 0x96 0x49 (4 байта)
```

> **Ключово предимство на двоичния формат:** при записи с фиксиран размер (напр. структури) можем директно да пресметнем байтовото отместване на всеки запис и да скочим към него – **пряк достъп**.

За да четем двоичен файл има няколко начина. За тези, които използват `VS Code` или `Visual Studio`, можете да изтеглите [Hex Editor](https://marketplace.visualstudio.com/items?itemName=ms-vscode.hexeditor). Намират се и уебсайтове, в които можете да поставяте файлове и да ги разглеждате в двоичен вид, например [HexEd.it](https://hexed.it/).

---

### Задача 1: Разлика в размера
Напишете програма, която записва едно и също голямо число (напр. `1000000`) 100 000 пъти веднъж в текстов файл и веднъж в двоичен файл. След това използвайте `tellp()` (или вашата операционна система), за да видите и сравните размера на двата файла в байтове.
*Шаблон:* [`code/01_compare_sizes.cpp`](code/01_compare_sizes.cpp)

## 3. Отваряне на двоичен файл

Единствената разлика спрямо текстов файл е добавянето на флага **`std::ios::binary`**:

```cpp
#include <fstream>

// Само за писане
std::ofstream out("data.bin", std::ios::binary);

// Само за четене
std::ifstream in("data.bin", std::ios::binary);

// За четене И писане (без изтриване на съдържанието)
std::fstream f("data.bin", std::ios::in | std::ios::out | std::ios::binary);
```

---

## 4. Писане – `write()`

```cpp
ostream& write(const char* buffer, std::streamsize count);
```

- `buffer` – указател към началото на данните, които ще се запишат
- `count` – брой байтове за запис
- Връща референция към потока (за верижно извикване)

> За да записваме данни в двоичен файл, те трябва да са стоят в паметта като отделни байтове. `const char*` е приет да се интерпретира като "указател към регион от паметта", защото е най-малкият адресируем тип (1 байт). Затова винаги трябва да правим `reinterpret_cast<const char*>` към данните, които искаме да запишем (освен ако данните, които записваме не са вече от тип `char*` или `const char*`).

### Запис на базов тип

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ofstream out("numbers.bin", std::ios::binary);
    if (!out) {
        std::cerr << "Грешка при отваряне!\n";
        return 1;
    }

    int a = 42;
    double b = 3.14;
    char c = 'Z';

    // reinterpret_cast<const char*> – третираме адреса на обекта като масив от байтове
    out.write(reinterpret_cast<const char*>(&a), sizeof(a));
    out.write(reinterpret_cast<const char*>(&b), sizeof(b));
    out.write(reinterpret_cast<const char*>(&c), sizeof(c));

    // Алтернативно чрез чейнване (по-рядко се използва):
    out.write(reinterpret_cast<const char*>(&a), sizeof(a))
       .write(reinterpret_cast<const char*>(&b), sizeof(b))
       .write(reinterpret_cast<const char*>(&c), sizeof(c));

    std::cout << "Записани " << (sizeof(a) + sizeof(b) + sizeof(c)) << " байта.\n";
    return 0;
}
```

### Запис на масив

```cpp
int arr[5] = {10, 20, 30, 40, 50};

std::ofstream out("array.bin", std::ios::binary);
// Записваме целия масив наведнъж – 5 * 4 = 20 байта
out.write(reinterpret_cast<const char*>(arr), sizeof(arr));
```

### Запис на символен низ

При запис на символен низ чрез указател (`char*` или `const char*`), трябва **първо да запишем дължината**, след което самия низ. Без предварително записана дължина, при четенето няма да знаем колко байта да прочетем.

> **Правилото важи за всеки указател към динамичен масив** (`int*`, `double*` и т.н.) – винаги записвайте броя елементи преди самите данни.

```cpp
// Статичен низ
const char* str = "Hello, world!";
size_t len = std::strlen(str);
out.write(reinterpret_cast<const char*>(&len), sizeof(size_t)); // първо дължината
out.write(str, len);                                             // после данните

// Динамичен низ
char* str_dynamic = new char[100];
std::strcpy(str_dynamic, "Something, something hello!");
size_t dynLen = std::strlen(str_dynamic);
out.write(reinterpret_cast<const char*>(&dynLen), sizeof(size_t)); // първо дължината
out.write(str_dynamic, dynLen);                                     // после данните

delete[] str_dynamic;
```

### Разлика между `reinterpret_cast` и `static_cast`:
- `reinterpret_cast` може да прави **произволни** преобразувания между несъвместими типове (напр. `int*` към `char*`), но е **опасен**, защото интерпретира типа като съвсем различен, без никакви проверки, което би довело до проблеми ако типовете са несъвместими.
- `static_cast` е по-строг и се използва за **безопасни** преобразувания (напр. `int` към `double`, или базов клас към производен), но не може да прави произволни преобразувания между несъвместими типове.

> **Защо не можем да правим `reinterpret_cast` на литерал?**  
> `reinterpret_cast<const char*>(5)` **не** взима адреса на стойността `5` – то интерпретира числото `5` директно като **адрес в паметта** (т.е. опитва се да чете памет на адрес `0x5`), което води до недефинирано поведение (най-вероятно `segfault`). За да запишем литерала `5`, трябва първо да го съхраним в променлива и да вземем нейния адрес:
> ```cpp
> int value = 5;
> out.write(reinterpret_cast<const char*>(&value), sizeof(int)); // правилно – &value е валиден адрес
> out.write(reinterpret_cast<const char*>(5),      sizeof(int)); // ГРЕШКА – 5 се третира като адрес 0x5
> ```

---

### Задача 2: Запис на различни данни
Напишете програма, която създава масив от 3 любими числа и един символен низ (динамичен или статичен). Запишете ги в двоичен файл `data.bin` - първо масива, а след това низа (не забравяйте да запишете дължината му първо!).
*Шаблон:* [`code/02_write_data.cpp`](code/02_write_data.cpp)

---

## 5. Четене – `read()`

```cpp
istream& read(char* buffer, std::streamsize count);
```

- `read()` чете точно `count` байта в `buffer`

> При четене ще използваме `reinterpret_cast<char*>` (без `const`), защото ще записваме в паметта, а не просто четем.

### Четене на базов тип

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("numbers.bin", std::ios::binary);
    if (!in) {
        std::cerr << "Грешка при отваряне!\n";
        return 1;
    }

    int    a;
    double b;
    char   c;

    in.read(reinterpret_cast<char*>(&a), sizeof(a));
    in.read(reinterpret_cast<char*>(&b), sizeof(b));
    in.read(reinterpret_cast<char*>(&c), sizeof(c));

    // Отново може да се чейне
    in.read(reinterpret_cast<char*>(&a), sizeof(a))
      .read(reinterpret_cast<char*>(&b), sizeof(b))
      .read(reinterpret_cast<char*>(&c), sizeof(c));

    std::cout << "a = " << a << "\n"  // 42
              << "b = " << b << "\n"  // 3.14
              << "c = " << c << "\n"; // Z
    return 0;
}
```

### Четене на масив

```cpp
int arr[5];

std::ifstream in("array.bin", std::ios::binary);
in.read(reinterpret_cast<char*>(arr), sizeof(arr));

if (in.gcount() == sizeof(arr)) {
    std::cout << "Прочетени всичките 5 числа.\n";
} else {
    std::cout << "Прочетени само " << in.gcount() / sizeof(int) << " числа.\n";
}
```

### Четене на символен низ

При четене първо четем записаната дължина, заделяме буфер и след това четем самия низ:

```cpp
size_t len;
in.read(reinterpret_cast<char*>(&len), sizeof(size_t)); // четем дължината
char* buffer = new char[len + 1];
in.read(buffer, len);                                    // четем низа
buffer[len] = '\0';                                     // добавяме нулев терминатор

// ... използваме buffer ...

delete[] buffer;
```

---

### Задача 3: Прочитане на данните
Напишете програма, която отваря създадения `data.bin` от Задача 2. Прочетете първо трите числа, изведете ги на екрана, след това прочетете дължината на низа, заделете буфер и прочетете самия низ. Използвайте `gcount()` за да проверите колко байта сте прочели за низа. 
*Шаблон:* [`code/03_read_data.cpp`](code/03_read_data.cpp)

---

## 6. Записване и четене на структури

Структурите се записват и четат **наведнъж** – целият обект като поредица от байтове.  

```cpp
#include <fstream>
#include <iostream>
#include <cstring>

struct Student {
    char   firstName[32];
    char   lastName[32];
    int    fn;
    double grade;
};
// sizeof(Student) е фиксиран – важно за пряк достъп!

void writeStudent(std::ofstream& out, const Student& s) {
    out.write(reinterpret_cast<const char*>(&s), sizeof(Student));
}

bool readStudent(std::ifstream& in, Student& s) {
    in.read(reinterpret_cast<char*>(&s), sizeof(Student));
    return in.gcount() == sizeof(Student); // успешно ли е прочетен?
}

int main() {
    std::ofstream out("students.bin", std::ios::binary);
    if (!out) { std::cerr << "Грешка!\n"; return 1; }

    Student s1 = {"Ivan",  "Ivanov",   12345, 5.50};
    Student s2 = {"Maria", "Georgieva",67890, 6.00};
    Student s3 = {"Petar", "Petrov",   11111, 4.75};

    writeStudent(out, s1);
    writeStudent(out, s2);
    writeStudent(out, s3);
    out.close();

    std::ifstream in("students.bin", std::ios::binary);
    if (!in) { std::cerr << "Грешка!\n"; return 1; }

    Student s;
    while (readStudent(in, s)) {
        std::cout << s.firstName << " " << s.lastName
                    << " | ФН: "     << s.fn
                    << " | Оценка: " << s.grade << "\n";
    }

    in.close();
    return 0;
}
```

> **Много важно:** Структури с **указателни членове** (`char*`, `int*` и т.н.) **не могат** да се записват наведнъж с `write(&s, sizeof(S))`, защото `sizeof(S)` отразява само размера на самия указател (8 байта на 64-битова система), а **не** данните, към които той сочи. При четене обратно бихме получили невалиден адрес вместо реалните данни. Затова всяко поле се записва поотделно с предварително записана дължина за динамичните масиви.
>
> Когато всички членове са примитивни типове или масиви с фиксиран размер, структурата може да се записва/чете наведнъж. Когато започнем да работим с класове, ще записваме всяка член данна поотделно, тъй като класовете съдържат и член функции, чиито адреси не желаем да записваме.

---

### Задача 4: Цветове
Създайте структура `Color` с 3 полета от тип `uint8_t`: `red`, `green`, `blue`. В програмата генерирайте масив от 100 цвята (например всички са изцяло червени `255, 0, 0`) и ги запишете наведнъж с `write`. После прочетете само първите 10 от файла в друг масив и ги изведете на екрана.
*Шаблон:* [`code/04_colors_rw.cpp`](code/04_colors_rw.cpp)

---

## 7. Пряк достъп (Random Access)

**Прякото (произволно) четене** позволява да отидем директно на произволна позиция в файла, **без да четем всичко преди нея**.

Това е възможно, когато всеки запис има **фиксиран размер** – каквато е всяка структура в C++.

### Формула за отместване на запис

```
offset = recordIndex * sizeof(RecordType)
```

### `seekg` и `seekp`

```cpp
// Примери за seekg (за четене):
in.seekg(0, std::ios::beg);   // начало на файла
in.seekg(0, std::ios::end);   // край на файла
in.seekg(n, std::ios::beg);   // n байта от началото
in.seekg(n, std::ios::cur);   // n байта напред от текущата позиция
in.seekg(-n, std::ios::cur);  // n байта назад
in.seekg(-n, std::ios::end);  // n байта преди края

// Аналогично за seekp (за писане)
```

### Четене на N-тия запис

```cpp
#include <fstream>
#include <iostream>

struct Student {
    char   firstName[32];
    char   lastName[32];
    int    fn;
    double grade;
};

// Чете запис с индекс 'index' (0-базиран)
bool readRecord(std::ifstream& in, size_t index, Student& s) {
    size_t offset = index * sizeof(Student);
    in.seekg(offset, std::ios::beg); // скачаме директно до позицията

    if (!in) return false; // seek извън файла

    in.read(reinterpret_cast<char*>(&s), sizeof(Student));
    return in.gcount() == sizeof(Student);
}

int main() {
    std::ifstream in("students.bin", std::ios::binary);
    if (!in) { std::cerr << "Грешка!\n"; return 1; }

    Student s;

    // Четем директно третия студент (индекс 2)
    if (readRecord(in, 2, s)) {
        std::cout << "Студент 3: " << s.firstName << " " << s.lastName << "\n";
    } else {
        std::cout << "Записът не съществува.\n";
    }

    return 0;
}
```

### Обновяване на N-тия запис (in-place update)

За промяна на конкретен запис без пренаписване на целия файл се ползва `fstream` с `in | out | binary`. Това работи, защото всеки запис е с **фиксиран размер** – новите данни заемат точно толкова байта, колкото оригиналните:

```cpp
#include <fstream>
#include <iostream>

struct Student {
    char   firstName[32];
    char   lastName[32];
    int    fn;
    double grade;
};

bool updateRecord(const char* filename, size_t index, const Student& newData) {
    std::fstream f(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!f) return false;

    size_t offset = index * sizeof(Student);
    f.seekp(offset, std::ios::beg);
    f.write(reinterpret_cast<const char*>(&newData), sizeof(Student));

    return f.good();
}

int main() {
    Student updated = {"Aleksandar", "Ivanov", 12345, 5.75};

    if (updateRecord("students.bin", 0, updated)) {
        std::cout << "Записът е обновен.\n";
    } else {
        std::cout << "Грешка при обновяване.\n";
    }

    return 0;
}
```

### Изтриване на запис – стратегии

Двоичните файлове **не поддържат истинско изтриване**. Има три подхода:

**Стратегия 1 – Флаг „изтрит"**

Добавяме поле `bool deleted` към структурата. Маркираме записа, без да го премахваме физически.

```cpp
struct Student {
    char   firstName[32];
    char   lastName[32];
    int    fn;
    double grade;
    bool   deleted;
};
```

При четене просто пропускаме записите с `deleted == true`.

**Стратегия 2 – Преместване на последния запис**

Копираме последния запис на мястото на изтрития, след което „съкращаваме" файла.

**Стратегия 3 – Презапис в нов файл**

Копираме всички валидни записи в нов файл и заменяме оригинала.

---

## 8. Изчисляване на брой записи

```cpp
std::ifstream in("students.bin", std::ios::binary);

in.seekg(0, std::ios::end);
std::streampos fileSize = in.tellg();
int recordCount = static_cast<int>(fileSize) / sizeof(Student);

std::cout << "Брой записи: " << recordCount << "\n";
```

> Работи **само ако** всички записи са с фиксиран и еднакъв размер.

---

### Задача 5: Модификация в средата
Отворете създадения файл с цветове от Задача 4, използвайки флаг `std::ios::in | std::ios::out | std::ios::binary`. Намерете броя на записаните цветове, използвайки `seekg(0, std::ios::end)` и `tellg()`. След това използвайте пряк достъп (`seekp()`), за да отидете на 50-тия цвят в файла и да го промените на зелен (`0, 255, 0`).
*Шаблон:* [`code/05_modify_color.cpp`](code/05_modify_color.cpp)

---

## 9. Обобщение

### Методи за двоично I/O

| Операция | Метод | Бележка |
|----------|-------|---------|
| Запис на блок байтове | `write(const char* buf, n)` | Ползва `reinterpret_cast` |
| Четене на блок байтове | `read(char* buf, n)` | Ползва `reinterpret_cast` |
| Брой прочетени байтове | `gcount()` | След `read()` |
| Позиция при четене | `tellg()` | Байтово отместване |
| Позиция при писане | `tellp()` | Байтово отместване |
| Преместване (четене) | `seekg(offset, dir)` | `beg` / `cur` / `end` |
| Преместване (писане) | `seekp(offset, dir)` | `beg` / `cur` / `end` |

### Формули за пряк достъп

| Операция | Формула |
|----------|---------|
| Offset на запис `i` | `i * sizeof(Record)` |
| Брой записи | `fileSize / sizeof(Record)` |
| Последен запис (индекс) | `recordCount - 1` |

### Кога да изберем двоичен файл?

- Работим с **числови данни** и размерът е важен
- Нужен е **пряк достъп** (четене/обновяване на конкретен запис)
- Скоростта на I/O е критична
- Данните не трябва да са четими от човек

### Кога да предпочетем текстов файл?

- Файлът трябва да се редактира ръчно
- Преносимост между платформи и езици
- Конфигурационни файлове, логове

## Задачи

Напишете задачата от [предишната седмица](https://github.com/Stelllarce/Object-Oriented-Programming-IS-2026/tree/main/Week_03##задачи) за работа с герои и куестове, но този път **записвайте данните в двоичен файл**.