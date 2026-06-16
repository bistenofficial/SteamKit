# Заметка по миграции SteamKitCpp

SteamKitCpp должен заменить `SteamKit2.dll` для native C++ consumers после
достижения full parity. Текущий SDK - foundation layer, поэтому в production
integrations нужно сохранять managed dependency до тех пор, пока native parity
suite не покроет нужное поведение.

## Целевая форма consumer

Установите или добавьте SDK package, затем линкуйте exported target:

```cmake
find_package(SteamKitCpp CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE SteamKitCpp::steamkit)
```

Подключите public SDK header:

```cpp
#include <steamkit/steamkit.hpp>
```

## Шаги миграции после parity

1. Добавить `SteamKitCpp` в native build.
2. Заменить managed SteamKit2 value types на `steamkit::SteamID`,
   `steamkit::GameID` и связанные native types.
3. Заменить callback pump code на `steamkit::CallbackManager`.
4. Заменить доступ к handlers через typed client accessors:
   `client.user()`, `client.friends()`, `client.apps()`,
   `client.game_coordinator()`, `client.unified_messages()` и `client.cdn()`.
5. Заменить raw message usage на `steamkit::ClientMsg<T>` и
   `steamkit::ClientGCMsg<T>`.
6. Убрать `.NET` runtime и `SteamKit2.dll` dependency из integration file только
   после того, как эквивалентный native flow пройдет parity tests.

## Вне области работ

Эта note намеренно generic. Она не требует просмотра application source code.
Downstream applications должны делать финальную замену dependency в своем
integration layer после того, как нужные им SteamKitCpp features реализованы и
проверены.
