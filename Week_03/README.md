# Файлове – основни концепции. Текстови файлове. Преглед на `fstream`

---

## Съдържание

1. [Какво е файл?](#1-какво-е-файл)
2. [Видове файлове](#2-видове-файлове)
3. [Поток (Stream) – абстракцията зад файловете](#3-поток-stream--абстракцията-зад-файловете)
4. [Заглавни файлове и йерархия на класовете](#4-заглавни-файлове-и-йерархия-на-класовете)
5. [Отваряне и затваряне на файл](#5-отваряне-и-затваряне-на-файл)
6. [Режими на отваряне](#6-режими-на-отваряне)
7. [Проверка на състоянието на потока](#7-проверка-на-състоянието-на-потока)
8. [Текстови файлове – четене и писане](#8-текстови-файлове--четене-и-писане)
9. [Навигация във файл](#9-навигация-във-файл)
10. [Чести грешки](#10-чести-грешки)
11. [Обобщение](#11-обобщение)
12. [Задачи](#задачи)

---

## 1. Какво е файл?

**Файлът** е именувана поредица от байтове, съхранена на траен носител (твърд диск, SSD и др.).  
За разлика от паметта (RAM), данните във файл **се запазват след края на програмата**.

Всеки файл има:
- **Име и разширение** – напр. `students.txt`
- **Размер** – брой байтове
- **Позиция (указател за четене/писане)** – откъде ще се извърши следващата операция
- **Метаданни** – дата на създаване, права за достъп и др.

### 1.1 Пътища до файлове

Windows
- `C:\Users\Bob\Documents\data.txt`
    - `C:\` - показва на кой диск е файла
    - използва '\\' като разделител на папки
    - case-insensitive (не прави разлика между главни и малки букви) - `data.txt` и `DATA.TXT` са един и същ файл

Linux/Mac
- `/home/bob/data.txt`
    - `/` - корен на файловата система
    - използва '/' като разделител на папки
    - case-sensitive (прави разлика между главни и малки букви) - `data.txt` и `DATA.TXT` са различни файлове

Специални пътища:
- `.` - текущата директория
- `..` - родителската директория

## 2. Видове файлове

| Вид | Съдържание | Примери |
|-----|-----------|---------|
| **Текстов** | Четими символи, редовете се разделят с `\n` (или `\r\n` на Windows, [ето защо](https://dev.to/pieter/why-windows-uses-rn-newlines-instead-of-n-126l)) | `.txt`, `.csv`, `.cpp` |
| **Двоичен (binary)** | Суров поток от байтове, без специална интерпретация | `.exe`, `.png`, `.bin` |

> В тази тема се фокусираме върху **текстовите файлове**.  
> Двоичните файлове ще разгледаме в следващо занятие.

## 3. Поток (Stream) – абстракцията зад файловете

C++ работи с файлове чрез концепцията **поток (stream)** – последователен канал за байтове.  
Потоците скриват детайлите на операционната система и предоставят единен интерфейс.

![](https://jupyternotebooks-dpb6-e8e09e2d24a591fa64baaf90dc750c5415a29cf5e.pages.oit.duke.edu/_images/stream.png)

Вече познатите ни `std::cin` и `std::cout` са точно такива потоци – само че свързани с конзолата, не с файл.

### 3.1 Буфериране

Когато пишете данни в поток, те **не се записват на диска незабавно**. Вместо това се натрупват в малка памет, наречена **буфер**, и се изпращат на диска като по-голям блок. Четенето работи аналогично – OS зарежда блок от диска в буфера и програмата чете от него.

**Защо се буферира?**  
Всяко физическо записване на диск е бавна системна операция. Натрупването на данни в паметта и записването им наведнъж е многократно по-ефективно.

**Кога буферът се изпразва (flush) автоматично?**

| Ситуация | Пример |
|----------|--------|
| Буферът е пълен | Записани са достатъчно данни |
| `close()` е извикан | `out.close()` |
| Потокът излезе от обхват (деструктор) | Края на блок `{}` |

**Кога трябва да го направите ръчно?**  
При извънредно прекратяване на програмата (`exit()`, срив) деструкторите не се викат и данните в буфера може да се изгубят. В такива случаи извикайте `out.flush()` преди изхода.

```cpp
std::ofstream log("app.log", std::ios::app);
log << "[INFO] Step completed\n";
log.flush(); // write to disk immediately, even if the buffer is not full
```

> `std::endl` прави `'\n'` **и** `flush()`. Затова е по-бавен от `"\n"` – използвайте `"\n"` освен ако не Ви трябва незабавно изпразване на буфера.

![Buffers](https://i.sstatic.net/02ho1.png)

---

## 4. Заглавни файлове и йерархия на класовете

```cpp
#include <fstream>   // ifstream, ofstream, fstream
#include <iostream>  // за cout / cerr / endl / cin
```

### Йерархия на класовете

![](https://jcodebook.com/wp-content/uploads/2023/12/FileStream-1-1024x617.jpg)

| Клас | Предназначение |
|------|---------------|
| `std::ifstream` | Отваря файл **само за четене** (input file stream) |
| `std::ofstream` | Отваря файл **само за писане** (output file stream) |
| `std::fstream`  | Отваря файл **за четене и писане** едновременно |

---

## 5. Отваряне и затваряне на файл

### Начин 1 – чрез конструктора

```cpp
#include <fstream>

int main() {
    // Here we create an object through its constructor, more on this in Week 4
    std::ifstream in("data.txt");   // opens file to read
    std::ofstream out("result.txt"); // opens for writing

    // ... something, somthing, files ...

    // Files are automatically closed when they go out of scope because a destructor is called (again, more on that in Week 4)
    return 0;
}
```

### Начин 2 – чрез `open()`

```cpp
#include <fstream>

int main() {
    std::ifstream in;
    in.open("data.txt");

    if (!in.is_open()) {
        // ... error handling ...
        return 1;
    }

    // ... something, somthing, file ...

    in.close(); // close the file explicitly
    return 0;
}
```
При отваряне на файл в който и да е режим, програмата ще търси името на файла в сегашната работна директория. Например:
\
Ако се намираме в `/home/user/` и пуснем `./projects/myapp/main`, то файлът data.txt ще бъде намерен успешно, защото работната папка е `/home/user/`, а не папката на изпълнимия файл. Ако обаче влезем в `home/user/projects/myapp/` и пуснем `./myapp`, то `data.txt` няма да бъде намерен, защото там няма такъв файл. В този случай трябва да посочим пътя до файла спрямо работната директория, например `../../data.txt`. Би било най-просто ако подадем абсолютен път до файла, например `/home/user/data.txt`, но това прави програмата по-малко преносима, защото на друг компютър може да няма папка `/home/user/` или файлът да е в друга папка. Затова най-често се използват относителни пътища спрямо работната директория.

```
/home/user/
├── projects/myapp/
│   └── main         ← the executable
└── data.txt
```
## 6. Режими на отваряне

Режимите се задават като втори аргумент на конструктора или `open()`:

| Флаг | Абревиатура | Значение |
|------|-------------|----------|
| `std::ios::in`     | **input** (data flows into the program) | Четене (подразбиращ се за `ifstream`) |
| `std::ios::out`    | **output** (data flows out of the program) | Писане (подразбиращ се за `ofstream`). Ако файл с такова име не съществува, се създава нов. В противен случай съдържанието на вече съществуващ файл **се изтрива при отваряне**.  |
| `std::ios::app`    | **append** | Добавяне в края на файла (append). **Съдържанието не се изтрива, но курсорът за писане стои залепен в края и не може да се мести.** |
| `std::ios::trunc`  | **truncate** | Изтрива съдържанието при отваряне (подразбиращ се при `out`) |
| `std::ios::ate`    | **at end** | Позицията се поставя в края веднага след отваряне. **В този режим курсорът може да се мести, за разлика от `app`** |
| `std::ios::binary` | **binary** | Двоичен режим |

Флаговете могат да се **комбинират с `|` (побитовото или)**:

```cpp
// Open a file for both reading and writing, without truncating it
std::fstream f("log.txt", std::ios::in | std::ios::out | std::ios::app);
```

>**Важно:** От C стандарта се изисква при редуване на операция за четене и писане от един и същи поток, трябва между тях или да се извърши отместване на курсора първо, или да се извика `flush()`. Това е така, защото при смесен режим потокът не знае дали следващата операция ще е за четене или писане и трябва да се увери, че всички буфери са синхронизирани преди да продължи. Референция: [reading-and-writing](https://stackoverflow.com/questions/17536570/reading-and-writing-to-the-same-file-using-the-same-fstream/17567454#17567454)

> Тези флагове отдолу са всъщност битови маски и затова се комбинират с `|` (побитовото или). Имплементирахме подобни в Week 1 при създаването на `Permissions` с `read`, `write` и `execute`, само че тук са член данни на клас.

### Честа грешка - добавяне на нови данни в непразен файл

```cpp
// Defautlts to ios::out | ios::trunc, file contents are deleted
std::ofstream f("important.txt");

// Appends to the end of file and does not truncate
std::ofstream f("important.txt", std::ios::app);
```
> **Запомнете**: основната разлика между `app` и `ate` е, че при `app` курсорът за писане е залепен в края и не може да се мести, докато при `ate` може да се мести свободно. Това е така, защото при `app` всяка операция за писане се предхожда автоматично от `seeek` операция за преместване в края.

## 7. Проверка на състоянието на потока

Всеки поток пази набор от вътрешни **флагове за грешки** (state bits).

| Метод | Значение |
|-------|---------|
| `good()`  | Всичко е наред – потокът е готов за операции |
| `eof()`   | Достигнат е краят на файла |
| `fail()`  | Последната операция е неуспешна (логическа или критична грешка) |
| `bad()`   | Критична грешка в I/O слоя (напр. проблем с диска) |
| `clear()` | Изчиства всички флагове |

### Константи на `iostate`

Всеки бит може да бъде зададен директно чрез `setstate()` или проверен чрез `rdstate()`. Таблицата показва как всяка константа влияе върху функциите за проверка:

| `iostate` константа | Значение | `good()` | `eof()` | `fail()` | `bad()` | `rdstate()` |
|---------------------|----------|:--------:|:-------:|:--------:|:-------:|:-----------:|
| `goodbit` | Няма грешки (нулева стойност) | `true` | `false` | `false` | `false` | `goodbit` |
| `eofbit`  | Достигнат е краят на файла при входна операция | `false` | `true` | `false` | `false` | `eofbit` |
| `failbit` | Логическа грешка при I/O операция | `false` | `false` | `true` | `false` | `failbit` |
| `badbit`  | Грешка при четене/писане при I/O операция | `false` | `false` | `true` | `true` | `badbit` |

### Потокът като булева стойност

Операторите `operator bool()` и `operator!` се базират на `fail()` и `bad()`:

```cpp
if (!in) { /* same as: if (in.fail() || in.bad()) */ }
if (in)  { /* same as: if (!in.fail() && !in.bad()) */ }
```

### 7.1 `good()` – потокът е напълно здрав

`good()` връща `true` само ако **нито един** флаг не е вдигнат. При всяка успешна операция за четене/писане потокът остава в добро състояние.

**Кога е `true`:** след успешна операция, преди четене на последния байт.  
**Кога е `false`:** веднага щом поне един от `eofbit`, `failbit` или `badbit` е вдигнат.

> Файл с примера: [`code/flags/trigger_good.cpp`](code/flags/trigger_good.cpp)  
> Данни: [`data/numbers.txt`](data/numbers.txt)

```cpp
// code/flags/trigger_good.cpp (excerpt)
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("data/numbers.txt"); // съдържа: 10 20 30 40 50

    // Преди четене – всички флагове са чисти
    std::cout << "good()=" << in.good() << "\n"; // 1

    int x;
    while (in >> x) {
        // good() е true за всяка итерация, в която четенето успява
        std::cout << "Read: " << x << "  good()=" << in.good() << "\n";
    }

    // След изчерпване на файла good() вече е false (eofbit е вдигнат)
    std::cout << "After loop: good()=" << in.good() << "\n"; // 0
    return 0;
}
```

---

### 7.2 `eof()` – край на файла

`eofbit` се вдига, когато операция за четене се опита да прочете **след** последния байт на файла.

> **Важно:** `eof()` не се вдига *преди* последното четене – вдига се *по време* на четенето, което достига края. Последната стойност пак се извлича успешно.

**Чести ситуации, в които се вдига:**

| Ситуация | Пример |
|----------|--------|
| Прочетени са всички токени с `>>` | `while (in >> x)` – след последния `x` |
| `getline` е достигнал края | `while (in.getline(buf, N))` – след последния ред |
| `read()` е прочел по-малко байтове от исканото | `in.read(buf, 1000)` върху файл с 200 байта |

> Файл с примера: [`code/flags/trigger_eof.cpp`](code/flags/trigger_eof.cpp)  
> Данни: [`data/numbers.txt`](data/numbers.txt)

```cpp
// code/flags/trigger_eof.cpp (excerpt)
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("data/numbers.txt");
    int x;

    while (in >> x) { /* reads 10 20 30 40 50 */ }

    std::cout << "eof()="  << in.eof()  << "\n"; // 1
    std::cout << "fail()=" << in.fail() << "\n"; // 1 (fail and eof triggered)

    // Reading after EOF
    in >> x; // nothing can be read, but x is unchanged

    // Recovering from EOF: clear the flags and seek back to the beginning
    in.clear();
    in.seekg(0, std::ios::beg);
    std::cout << "After recovery: good()=" << in.good() << "\n"; // 1

    while (in >> x) { std::cout << "Re-read: " << x << "\n"; }
    return 0;
}
```

---

### 7.3 `fail()` – логическа грешка при операцията

`failbit` се вдига при **неуспешна форматирана операция** – операцията не може да се изпълни, но потокът и буферът остават в консистентно състояние (т.е. грешката е **повъзстановима**).

**Чести ситуации, в които се вдига:**

| Ситуация | Пример |
|----------|--------|
| Несъответствие на типа | `in >> intVar` среща `"hello"` |
| Четене след достигнат EOF | всяко `>>` след края на файла |
| Файлът не може да се отвори (`ifstream`) | `std::ifstream in("nonexistent.txt")` |
| `getline` при неотворен поток | четене без успешен `open()` |

> Файл с примера: [`code/flags/trigger_fail.cpp`](code/flags/trigger_fail.cpp)  
> Данни: [`data/mixed.txt`](data/mixed.txt)

```cpp
// code/flags/trigger_fail.cpp (excerpt)
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("data/mixed.txt"); // 42 \n hello \n 99

    int x;
    in >> x;  // reads 42 successfully
    std::cout << "x=" << x << " fail()=" << in.fail() << "\n"; // 42, 0

    in >> x;  // tries to read "hello" as int -> FAILBIT
    std::cout << "fail()=" << in.fail() << "\n"; // 1
    std::cout << "x (unchanged)=" << x << "\n"; // 42 – old value

    // Recovery
    in.clear();
    char bad_token[64];
    in >> bad_token;      // consume "hello"

    in >> x;              // reads 99 successfully
    std::cout << "x=" << x << " fail()=" << in.fail() << "\n"; // 99, 0
    return 0;
}
```

---

### 7.4 `bad()` – критична грешка в I/O слоя

`badbit` се вдига при **системни/хардуерни грешки** в самия поток – загубена връзка с файловата система, повреден буфер, диск изпаднал по средата на операция. За разлика от `failbit`, `badbit` е **неповъзстановим** – stream-ът вече не е надежден.

**Чести ситуации, в които се вдига:**

| Ситуация | Пример |
|----------|--------|
| Хардуерна грешка средата на четене/писане | Диск изпада по средата на `write()` |
| Четене от файл, изтрит след отваряне (на някои OS) | `unlink()` докато файлът е отворен и буфериран |
| Повреда на вътрешния буфер на потока | Нулиран буфер чрез `rdbuf()->setbuf(nullptr, 0)` |

### 7.4 Как да се справим с всяка ситуация

```cpp
std::ifstream in("data/numbers.txt");

if (!in) {
    std::cerr << "Cant open file\n";
    return 1;
}

int x;
while (in >> x) {
    // do something to x
}

if (in.bad()) {
    // Stop immediately – the stream is unreliable
    std::cerr << "[FATAL] I/O error.\n";
    return 1;
}

if (in.eof()) {
    // We can recover from EOF by clearing the flags and seeking back to the beginning
    in.clear();
    in.seekg(0, std::ios::beg);
}

if (in.fail() && !in.eof()) {
    // Bad token encountered – we can skip it and continue
    in.clear();
    char skip[64];
    in >> skip;  // discard the bad token
}
```

> Следвайте следните стъпки при работа с файлове: 
> 1. Създайте обект на потока (`ifstream`, `ofstream` или `fstream`).
> 2. Отворете файла (чрез конструктор или `open()`).
> 3. Проверете дали файлът е отворен успешно с `is_open()`.
> 4. Извършете необходимите операции (четене/писане).
> 5. Затворете файла с `close()`.
> 6. Проверете за грешки с `!file` или `file.fail()` и реагирайте съответно.

## 8. Текстови файлове – четене и писане

### 8.1. Писане с `<<`

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ofstream out("students.txt");

    if (!out) {
        std::cerr << "Cant open output file\n";
        return 1;
    }

    // Write some student records in the format: FirstName LastName FN Grade
    out << "Ivan Ivanov"  << " " << 12345 << " " << 5.50 << "\n";
    out << "Maria Georgieva" << " " << 67890 << " " << 6.00 << "\n";
    out << "Petar Petrov" << " " << 11111 << " " << 4.75 << "\n";

    // out.close() is called automatically by the destructor (which also flushes)
    // Calling close() explicitly is still recommended
    out.close();

    if (out.fail()) {
        std::cerr << "Write error!\n";
        return 1;
    }

    return 0;
}
```

> **`flush()` vs `close()`:** `close()` автоматично изчиства буфера преди затваряне. `flush()` обаче го прави **веднага** без да затваря файла – полезно когато трябва да e сигурно записано на диск, докато файлът остава отворен (напр. лог файлове или данни след всяка транзакция).

**Съдържание на `students.txt` след изпълнение:**
```
Ivan Ivanov 12345 5.5
Maria Georgieva 67890 6
Petar Petrov 11111 4.75
```

---

### 8.2. Четене с `>>` (по думи/числа)

Операторът `>>` пропуска **whitespace** (интервали, табове, нови редове) и чете до следващия такъв.

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("students.txt");

    if (!in) {
        std::cerr << "Cant open input file\n";
        return 1;
    }

    char firstName[64], lastName[64];
    int fn;
    double grade;

    while (in >> firstName >> lastName >> fn >> grade) {
        std::cout << "Студент: " << firstName << " " << lastName
                  << ", ФН: "   << fn
                  << ", Оценка: " << grade << "\n";
    }

    return 0;
}
```

**Изход:**
```
Студент: Ivan Ivanov, ФН: 12345, Оценка: 5.5
Студент: Maria Georgieva, ФН: 67890, Оценка: 6
Студент: Petar Petrov, ФН: 11111, Оценка: 4.75
```

---

### 8.3. Четене ред по ред с `getline`

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("students.txt");

    if (!in) {
        std::cerr << "Cant open input file\n";
        return 1;
    }

    char line[256];
    int lineNumber = 0;

    while (in.getline(line, sizeof(line))) {
        ++lineNumber;
        std::cout << lineNumber << ": " << line << "\n";
    }

    return 0;
}
```

> `in.getline(buffer, size)` чете до символа `\n` (или до `size-1` символа) и **не включва `\n`** в буфера. Това може да се промени с третия аргумент на `getline()`, но по подразбиране е `\n`. Например `in.getline(line, sizeof(line), ';')` ще чете до `;` вместо до нов ред.

---

### 8.4. Четене и писане символ по символ с `get` и `put`

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("source.txt");
    std::ofstream out("destination.txt");

    if (!in || !out) {
        std::cerr << "Cant open input or output file\n";
        return 1;
    }

    int count = 0;
    char ch;

    while (in.get(ch)) {   // reads one character at a time, including whitespace '\n', ' ', etc.
        out.put(ch);
        ++count;
    }

    std::cout << "Copied " << count << " symbols.\n";
    return 0;
}
```

### Разлика между `>>` и `get()`

| | `>>` | `get()` |
|--|------|---------|
| Пропуска whitespace | Да | **Не** |
| Чете `\n` | Не | **Да** |
| Подходящ за | Форматирани данни | Суров текст, копиране |

---

### 8.5. `peek` и `unget`

```cpp
std::ifstream in("data.txt");
char ch;

// reads next symbol, but does not remove it from the stream
if (in.peek() == '#') {
    // We found a comment line – skip it
    char dummy[256];
    in.getline(dummy, sizeof(dummy));
} else {
    in >> ch;
}
```

```cpp
char c = in.get();
if (c == '-') {

} else {
    // returns the last read character back to the stream, so it can be read again
    in.unget();
    int n;
    in >> n;
}
```

---

### 8.6. `gcount()` – брой действително прочетени байтове

`gcount()` връща броя символи (`std::streamsize`), прочетени от **последната неформатирана входна операция** (`<<`, `>>`, `std::getline`). Полезно е с `getline`, за да разберете колко символа са прочетени от реда (без `\0`).

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("data/numbers.txt");

    char buf[64];
    while (in.getline(buf, sizeof(buf))) {
        std::streamsize n = in.gcount();
        std::cout << "Line (" << n << " chars): " << buf << "\n";
    }

    return 0;
}
```

| Операция | `gcount()` връща |
|----------|----------------|
| `in.get(ch)` – успешно | `1` |
| `in.getline(buf, N)` | брой прочетени символа без терминиращия `\0` |
| `in >> x` (форматирано четене) | **винаги `0`** – не е небуферирана операция |

---

### 8.7. `rdbuf()` – директен достъп до вътрешния буфер

`rdbuf()` връща указател към `std::streambuf` обекта, който стои зад потока. Малко са ситуациите, в които би послужил, но да обърна внимание че съществува.

Бърз пример за употреба на `rdbuf()` е при копиране на един цял файл в друг:

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("source.txt");
    std::ofstream out("source_copy.txt");

    if (!in || !out) {
        std::cerr << "Cant open input or output file\n";
        return 1;
    }

    out << in.rdbuf();

    return 0;
}
```

## 9. Навигация във файл

Всеки поток поддържа **указател за четене** (`g` – get) и **указател за писане** (`p` – put).

| Метод | Описание |
|-------|---------|
| `tellg()` | Връща текущата позиция на четене (в байтове от началото). Върнатия тип е `std::streampos` или еквивалетно `pos_type`|
| `tellp()` | Връща текущата позиция на писане. Върнатия тип е `std::streampos` или еквивалетно `pos_type` |
| `seekg(pos)` | Премества четящия указател на абсолютна позиция |
| `seekg(offset, dir)` | Премества с `offset` байта спрямо `dir` |
| `seekp(pos)` | Аналогично за пишещия указател |
| `seekp(offset, dir)` | Аналогично за пишещия указател с `offset` и `dir` |

**Посоки (`dir`):**

| Константа | Значение |
|-----------|---------|
| `std::ios::beg` | От началото на файла |
| `std::ios::cur` | От текущата позиция |
| `std::ios::end` | От края на файла |

### Пример – намиране на размера на файл

```cpp
#include <fstream>
#include <iostream>

int main() {
    std::ifstream in("data.txt");

    if (!in) {
        std::cerr << "Error when opening file!\n";
        return 1;
    }

    in.seekg(0, std::ios::end);
    std::streampos fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::cout << "File size: " << fileSize << " bytes\n";

    return 0;
}
```

### Пример – четене на N-тия ред

```cpp
#include <fstream>
#include <iostream>

// Reads the targetLine-th line from the file into buffer. Returns true if successful, false if EOF is reached first.
bool readLine(std::ifstream& in, int targetLine, char* buffer, int bufSize) {
    in.seekg(0, std::ios::beg);
    in.clear(); // clear flags like eof

    int current = 0;
    while (in.getline(buffer, bufSize)) {
        ++current;
        if (current == targetLine) {
            return true;
        }
    }
    return false;
}
```

## 10. Чести грешки

### Грешка 1 – Не се проверява дали файлът е отворен

```cpp
// ГРЕШНО
std::ifstream in("nonexistent.txt");
int x;
in >> x; // undefined behavior – операцията на затворен/невалиден поток

// ПРАВИЛНО
std::ifstream in("nonexistent.txt");
if (!in) {
    std::cerr << "Error when opening file!\n";
    return 1;
}
int x;
in >> x;
```

### Грешка 2 - четене в цикъл до eof

```cpp
// ГРЕШНО: последната итерация ще е неуспешна, но x ще е променен от невалидното четене
int x;
while (!in.eof()) {
    in >> x;
    std::cout << "Read: " << x << "\n"; // последният x ще е невалиден, но все пак се печата
}

// ПРАВИЛНО: четене до неуспех, който може да е EOF или друга грешка
int x;
while (in >> x) {
    std::cout << "Read: " << x << "\n";
}
```

### Грешка 3 – Смесване на `>>` и `getline`

```cpp
// ПРОБЛЕМ: >> оставя '\n' в буфера; следващото getline чете празен ред
int n;
in >> n;               // чете числото, '\n' остава в буфера
in.getline(buf, 256);  // чете "" (само новия ред!)

// РЕШЕНИЕ: игнорирай остатъка от реда преди getline
in >> n;
in.ignore(1000, '\n'); // пропусни до края на реда
in.getline(buf, 256);  // сега чете правилно
```

### Грешка 4 – Четене след EOF без `clear()`

```cpp
std::ifstream in("data.txt");
// ... четем до края ...
in.seekg(0, std::ios::beg); // ГРЕШНО: eof флагът е вдигнат, seekg не помага

in.clear();                  // ПРАВИЛНО: първо изчисти флаговете
in.seekg(0, std::ios::beg); // след това се върни в началото
```

### Грешка 5 – Забравено затваряне при `ofstream`
Може да доведе до няколко типа проблеми
```cpp
std::ofstream out("output.txt");
out << "some data\n";

// Scenario 1: program crashes and buffers are unflushed
exit(1); // buffers may not be flushed, file may be corrupted
// What to do instead: flush the stream after writing or just close it
out.flush(); // forces the buffer to be written to the file
out.close(); // also flushes the buffer and releases resources

// Scenario 2: the file is being opened from another stream object within the same scope, which leads to undefined behavior
std::ofstream out2("output.txt"); // undefined behavior – out is still open and may have unflushed data
```

---

## 11. Обобщение

| Клас | Кога да използвам |
|------|------------------|
| `std::ifstream` | Само четене от файл |
| `std::ofstream` | Само писане/създаване на файл |
| `std::fstream`  | Едновременно четене и писане |

| Операция | Метод |
|----------|-------|
| Четене на форматирани данни | `>>` |
| Писане на форматирани данни | `<<` |
| Четене на ред | `getline(buf, size, delimiter)` |
| Четене на символ | `get(ch)` |
| Писане на символ | `put(ch)` |
| Четене на блок байтове | `read(buf, n)` |
| Писане на блок байтове | `write(buf, n)` |
| Брой прочетени байтове | `gcount()` |
| Достъп до вътрешния буфер | `rdbuf()` |
| Позиция при четене | `tellg()` / `seekg()` |
| Позиция при писане | `tellp()` / `seekp()` |
| Проверка за край на файл | `eof()` |
| Изчистване на флагове | `clear()` |
| Принудително изпразване на буфера | `flush()` |
| Затваряне (+ flush) | `close()` |
| Отваряне с режим | `open(filename, mode)` |
| Игнориране на символи | `ignore(n, delimiter)` |

### Допълнителни четива
[Why windows uses \r\n instead of \n for newlines](https://dev.to/pieter/why-windows-uses-rn-newlines-instead-of-n-126l)

## Задачи

Файловете за задачата се намират в [`code/template/`](code/template/).

---

### Наемническата гилдия

Управлявате наемническа гилдия и трябва да запазвате и зареждате данни за героите и куестовете от файлове.

#### Част 1 – `Hero` и `Roster`

Структурата `Hero` представлява един герой с еднословно **име** с произволна дължина (без интервали – напр. `"Aragorn"`), **здраве** и **атака**. Структурата `Roster` е колекция от герои.

**Имплементирайте следните функции в `hero.cpp`:**

- `init` / `destroy` – заделяне и освобождаване на паметта за `name`
- `write` – записва герой във файл `heroes.txt` в следния формат:
  ```
  Aragorn 120 30
  ```
- `read` – чете герой от `heroes.txt` в същия формат

**Имплементирайте следните функции в `roster.cpp`:**

- `init` / `destroy` – заделяне и освобождаване на паметта (не забравяйте `destroy` за всеки герой)
- `addHero` – добавя герой в масива с **дълбоко копие** (отделена памет за `name`)
- `saveToFile` – записва roster в следния формат:
  ```
  Fellowship 3
  Aragorn 120 30
  Legolas 90 45
  Gimli 150 25
  ```
- `loadFromFile` – зарежда roster от файл

#### Част 2 – `Quest`

Структурата `Quest` има **заглавие** и **описание**, и двете с произволна дължина и **могат да съдържат интервали**, **награда** и **незадължителна бонус награда** (липсата на бонус трябва да се представи с подходяща стойност).

**Имплементирайте следните функции в `quest.cpp`:**

- `init` / `destroy`
- `write` – записва quest във файл в **избран от вас формат**
- `read` – чете quest от файл в **същия формат**

> **Обърнете внимание:** Заглавието и описанието съдържат интервали. Как ще разграничите едното от другото при четене? Как ще представите липсата на бонус награда?

#### Допълнение - няма тестове за тази част, имплемнтирайте я по желание
Имплементирайте функция или структура, която да проверява дали даден текстов файл с герои или куестове е в правилния формат и да извежда съобщение за грешка, ако не е. Например, ако файлът с герои съдържа ред като `Gandalf 100` (липсващ параметър за атака), програмата трябва да сигнализира за грешка и да посочи реда, на който се намира проблемът.

