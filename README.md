# Redis Lite

Учебное in-memory Key-Value хранилище на C++17. Сервер слушает TCP-порт, принимает текстовые команды и хранит данные в памяти процесса.

## Быстрый старт

Основной способ сборки:

```bash
cmake -S . -B build
cmake --build build
./build/redis-lite 6379
```

Если `cmake` не установлен, можно собрать тем же компилятором через `make`:

```bash
make
./build/redis-lite 6379
```

В другом терминале:

```bash
nc localhost 6379
```

Пример команд:

```text
PING
SET name Ivan
GET name
EXISTS name
DEL name
GET name
```

## Тестирование

Проект поддерживает unit-тесты на GoogleTest и отдельные сценарные мини-программы.

Сборка тестов:

```bash
cmake -S . -B build
cmake --build build
```

Запуск unit-тестов:

```bash
ctest --test-dir build --output-on-failure
```

## Как проверить транзакцию

```text
SET name Ivan
BEGIN
SET name Petr
GET name
ROLLBACK
GET name
```

Ожидаемый результат: внутри транзакции `GET name` вернет `Petr`, после `ROLLBACK` снова вернется `Ivan`.

## Как проверить подписки

Откройте два подключения через `nc`.

Первый клиент:

```text
SUBSCRIBE name
```

Второй клиент:

```text
SET name Maria
```

Первый клиент получит:

```text
NOTIFY name Maria
```