# SteamKitCpp

SteamKitCpp - это основа нативного C++20 SDK для SteamKit. SDK оформлен как
Windows x64 static library с публичными заголовками, CMake presets, vcpkg
manifest, тестами, samples и CMake package export.

Текущий код - foundation layer, а не полный parity-порт SteamKit2. Он покрывает
публичную форму SDK, core value types, raw message containers, callback routing,
handler accessors, package layout и entrypoint для генерации. Network logon, CM
transport, protobuf services, CDN downloads, WebAPI wrappers и полноценный Game
Coordinator пока намеренно оставлены как явные `NotImplemented` пути до переноса
соответствующих parity tests.

## Быстрый старт

Конфигурация, сборка и тесты без обязательных внешних зависимостей:

```powershell
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release
ctest --preset windows-x64-release
```

Конфигурация, сборка и тесты через vcpkg:

```powershell
$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
cmake --preset windows-x64-release-vcpkg
cmake --build --preset windows-x64-release-vcpkg
ctest --preset windows-x64-release-vcpkg
```

Установка и подключение package target:

```cmake
find_package(SteamKitCpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE SteamKitCpp::steamkit)
```

Подключение aggregate public header:

```cpp
#include <steamkit/steamkit.hpp>
```

## Публичный API

Foundation layer сейчас предоставляет:

- `steamkit::SteamID`, `steamkit::GameID`, `steamkit::JobID`
- `steamkit::Result<T>` и `steamkit::AsyncJob<T>`
- `steamkit::Client`, `steamkit::Configuration`, `steamkit::CallbackManager`
- typed callback subscriptions с RAII unsubscription
- raw `ClientMsg<TBody>` and `ClientGCMsg<TBody>` containers
- handler accessors: `user()`, `friends()`, `apps()`, `game_coordinator()`,
  `unified_messages()` и `cdn()`
- entrypoints для generated protocol namespaces через SteamLanguage codegen

## Документация

- [Индекс документации](../docs/STEAMKITCPP.md)
- [Сборка и тесты](../docs/STEAMKITCPP_BUILDING.md)
- [Описание API](../docs/STEAMKITCPP_API.md)
- [Архитектура](../docs/STEAMKITCPP_ARCHITECTURE.md)
- [Генерация кода](../docs/STEAMKITCPP_CODEGEN.md)
- [Тестирование](../docs/STEAMKITCPP_TESTING.md)
- [Секреты и безопасность live-аккаунтов](../docs/STEAMKITCPP_SECURITY.md)
- [Дорожная карта parity](../docs/STEAMKITCPP_PARITY_ROADMAP.md)
- [Заметка по миграции](../docs/STEAMKITCPP_MIGRATION.md)

## Структура

```text
SteamKitCpp/
  include/steamkit/      Публичные C++20 headers.
  src/                   Реализация static library.
  tests/                 Catch2, smoke, generator и safety tests.
  samples/               Минимальные consumer examples.
  tools/                 Нативные code generation tools.
  cmake/                 Package config templates.
  vcpkg.json             Manifest-mode dependencies.
```

## Безопасность

Не коммитьте Steam credentials, `.maFile` files, access tokens, cookies или
live-account fixtures. Репозиторный `.gitignore` блокирует типовые secret file
patterns, а `SteamKitCppSecretIgnoreTests` проверяет эти правила без чтения
реальных файлов аккаунтов.
