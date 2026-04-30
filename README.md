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

## Поддерживаемые команды

| Команда | Что делает | Ответ |
|---|---|---|
| `PING` | Проверяет соединение | `PONG` |
| `SET key value` | Сохраняет значение | `OK` |
| `GET key` | Читает значение | значение или `(nil)` |
| `DEL key` | Удаляет ключ | `OK` или `(nil)` |
| `EXISTS key` | Проверяет наличие ключа | `1` или `0` |
| `BEGIN` | Начинает транзакцию | `OK` |
| `COMMIT` | Подтверждает транзакцию | `OK` |
| `ROLLBACK` | Откатывает команды транзакции | `OK` |
| `SUBSCRIBE key` | Подписывает клиента на изменения ключа | `OK` |

Значения читаются как одно слово: `SET name Ivan` работает, а `SET name Ivan Petrov` сейчас не поддерживается.

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

## Что где лежит

- `include/storage` и `src/storage` — потокобезопасное хранилище, Singleton, LRU/FIFO eviction.
- `include/commands` и `src/commands` — команды через паттерн Command.
- `include/transaction` и `src/transaction` — история команд для `ROLLBACK`.
- `include/middleware` и `src/middleware` — проверка команд и маршрутизация.
- `include/session` и `src/session` — работа с одним клиентом.
- `include/server` и `src/server` — TCP-сервер, который принимает подключения.

