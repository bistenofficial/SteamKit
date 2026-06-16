# Дорожная карта parity SteamKitCpp

SteamKitCpp должен достичь feature parity со SteamKit2 по слоям. Каждый layer
должен добавлять native implementation, public API docs и parity tests до того,
как следующий слой начнет от него зависеть.

## Layer 1: SDK foundation

Статус: started.

Реализовано:

- static library target
- public header layout
- CMake presets and package exports
- vcpkg manifest
- `SteamID`, `GameID`, `JobID`
- `Result<T>` and `AsyncJob<T>` shells
- `Client`, `Configuration`, and `CallbackManager`
- форма handler accessors
- raw `ClientMsg<T>` and `ClientGCMsg<T>` containers
- SteamLanguage enum generator entrypoint
- smoke, Catch2, generator, and secret-ignore tests

Осталось:

- public API review для naming parity и long-term ABI expectations
- более широкое SteamID и GameID fixture coverage
- binary reader и writer primitives

## Layer 2: Binary IO, crypto и compression

Перенести:

- endian-aware binary readers и writers
- hash helpers
- symmetric и asymmetric crypto helpers
- zlib, zstd, lzma и zip helpers
- depot chunk checksum и decompression paths

Тесты:

- known digest vectors
- compression round trips
- depot chunk fixture parity

## Layer 3: Packets и client messages

Перенести:

- packet headers
- protobuf message wrappers
- encrypted message handling
- message serialization и deserialization
- server list packet behavior

Тесты:

- golden fixtures из SteamKit2 packet tests
- serialize и deserialize parity checks

## Layer 4: CM Networking

Перенести:

- TCP transport
- UDP transport
- WebSocket transport
- connection manager selection и retry behavior
- session lifecycle и heartbeat behavior

Тесты:

- transport parser unit tests
- deterministic connection state tests
- mocked CM tests перед live tests

## Layer 5: Authentication и user handler

Перенести:

- logon details
- access-token based auth
- two-factor challenge flow
- sentry handling
- logoff и reconnect behavior

Тесты:

- offline request construction tests
- mocked callback sequence tests
- opt-in live smoke tests со строгими safety rules

## Layer 6: Handlers

Перенести:

- friends и persona state
- apps и licenses
- Game Coordinator send и receive
- unified messages
- WebAPI helpers

Тесты:

- callback parity
- request URI и payload parity
- GC raw message fixture parity

## Layer 7: CDN и depot manifests

Перенести:

- CDN server discovery
- depot key handling
- manifest parsing
- chunk metadata
- chunk download и decompression

Тесты:

- depot manifest fixture parity
- chunk checksum parity
- URI construction parity

## Layer 8: Generated protocol surface

Перенести:

- SteamLanguage structured messages
- protobuf C++ generation
- public generated namespaces там, где SteamKit2 их экспортирует

Тесты:

- generator snapshot tests
- compile tests для public generated namespaces
- binary compatibility fixtures там, где применимо

## Финальный cleanup

Только после прохождения native parity tests:

- обновить consumers на линковку `SteamKitCpp::steamkit`
- убрать `.NET` и `SteamKit2.dll` dependency из integration files
- оставить generic migration notes для downstream C++ consumers
- держать SteamKit2 в репозитории, пока maintainers не решат, что его можно
  безопасно удалить
