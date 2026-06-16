# Документация SteamKitCpp

SteamKitCpp - это основа нативного C++20 SDK для SteamKit. После реализации и
проверки parity он должен стать самостоятельной C++ заменой для потребителей,
которым сейчас нужен SteamKit2.

Текущий foundation фиксирует package shape SDK и стабильные границы публичного
API:

- public headers в `SteamKitCpp/include/steamkit`
- static library target `SteamKitCpp`
- CMake alias target `SteamKitCpp::steamkit`
- CMake presets для Windows x64
- dependency resolution через vcpkg manifest
- installable CMake package config
- core Steam value types и raw message containers
- typed callback routing и handler accessors
- SteamLanguage code generation entrypoint
- C++ tests, C# oracle tests и проверки secret-file safety

## Текущий статус

SteamKitCpp пока не является полным parity-портом SteamKit2. Для network и
service layers, которые еще не перенесены, намеренно используются явные
`steamkit::NotImplemented` ошибки:

- CM connection и session management
- TCP, UDP и WebSocket transports
- authentication и logon flows
- encrypted channel setup
- protobuf generated message classes
- unified messages и WebAPI wrappers
- CDN depot manifest и chunk download logic
- Game Coordinator send/receive plumbing

Управляемый проект `SteamKit2` остается в репозитории как reference oracle до
тех пор, пока native parity suite не покроет старое поведение.

## Карта документации

- [Сборка и тесты](STEAMKITCPP_BUILDING.md)
- [Описание API](STEAMKITCPP_API.md)
- [Архитектура](STEAMKITCPP_ARCHITECTURE.md)
- [Генерация кода](STEAMKITCPP_CODEGEN.md)
- [Тестирование](STEAMKITCPP_TESTING.md)
- [Секреты и безопасность live-аккаунтов](STEAMKITCPP_SECURITY.md)
- [Дорожная карта parity](STEAMKITCPP_PARITY_ROADMAP.md)
- [Заметка по миграции](STEAMKITCPP_MIGRATION.md)

## Поддерживаемая платформа

Первый поддерживаемый target - Windows x64 с Visual Studio 2022 Build Tools или
Visual Studio 2022, CMake и vcpkg manifest mode. Другие платформы считаются
future work, пока transport, crypto, filesystem и package layers не получат
cross-platform tests.

## Контракт для consumer

Нативные consumers должны подключать:

```cpp
#include <steamkit/steamkit.hpp>
```

и линковать:

```cmake
find_package(SteamKitCpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE SteamKitCpp::steamkit)
```

Aggregate header - предпочтительный публичный include. Отдельные headers
остаются public для advanced consumers, но новые public API также должны быть
доступны через `steamkit/steamkit.hpp`.
