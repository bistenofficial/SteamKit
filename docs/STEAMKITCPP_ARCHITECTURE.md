# Архитектура SteamKitCpp

SteamKitCpp организован как самостоятельный SDK внутри существующего
репозитория SteamKit. Managed-проект SteamKit2 остается нетронутым и
используется как behavior oracle во время порта.

## Слои

Нативный порт должен расти слоями:

1. Core value types и binary IO
2. Crypto и compression
3. Message packets и raw client messages
4. CM networking через TCP, UDP и WebSocket
5. Callback и job system
6. User, friends, apps, Game Coordinator и unified message handlers
7. CDN и WebAPI clients
8. Generated protocol и protobuf types
9. Samples, install packaging и migration helpers

Текущий foundation реализует package shell и первые стабильные public API
boundaries, чтобы следующие слои можно было добавлять без перелома consumer code.

## Структура директорий

```text
SteamKitCpp/
  include/steamkit/      Public headers. Они задают SDK contract.
  src/                   Implementation files для static library.
  tests/                 Native tests и PowerShell safety probes.
  samples/               Минимальные consumer applications.
  tools/                 Native generation tools.
  cmake/                 CMake package config templates.
  vcpkg.json             Manifest-mode dependency declaration.
```

## Public Headers

`include/steamkit/steamkit.hpp` - aggregate include. Новые public SDK types
должны быть доступны через этот header.

Отдельные headers группируют API по ответственности:

- `types.hpp`: `SteamID`, `GameID`, `JobID`
- `configuration.hpp`: client configuration and protocol flags
- `callbacks.hpp`: `CallbackMsg`, `CallbackManager`, `Subscription`
- `messages.hpp`: raw `ClientMsg<T>` and `ClientGCMsg<T>`
- `handlers.hpp`: handler class declarations and callback shells
- `client.hpp`: `steamkit::Client`
- `result.hpp`: `Result<T>`
- `async_job.hpp`: `AsyncJob<T>`
- `enums.hpp`: foundational public enums
- `exception.hpp`: SDK exception types

## Error Boundary

APIs, которые входят в целевую public shape, но еще не реализованы, бросают
`steamkit::NotImplemented`. Это сделано намеренно: consumers и tests могут
компилироваться против native shape, но runtime не создает ложного ощущения
готовности.

Когда feature переносится, явный `NotImplemented` path заменяется реальным
поведением, а рядом добавляются parity tests против SteamKit2 fixtures.

## Generated Protocol Types

Generated namespaces должны оставаться public там, где SteamKit2 сегодня
экспортирует protocol types. Generated C++ code должен строиться из исходных
`.steamd` или `.proto` inputs, а не ручным переносом generated C# files.

## Package Model

SDK экспортирует:

- build target: `SteamKitCpp`
- CMake alias: `SteamKitCpp::steamkit`
- install package: `SteamKitCppConfig.cmake`

Consumers не должны линковать internal object files или подключать headers из
`src`.
