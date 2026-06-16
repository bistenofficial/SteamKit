# Описание API SteamKitCpp

Все публичные SDK-типы находятся в namespace `steamkit`. Предпочтительный
include - aggregate header:

```cpp
#include <steamkit/steamkit.hpp>
```

## Configuration

`steamkit::Configuration` описывает client-level defaults:

```cpp
steamkit::Configuration configuration;
configuration.protocols =
    steamkit::ProtocolTypes::Tcp | steamkit::ProtocolTypes::WebSocket;
configuration.connection_timeout = std::chrono::seconds(10);
configuration.web_api_base_address = "https://api.steampowered.com/";
```

`steamkit::ProtocolTypes` - bitmask enum. Для проверки enabled transports
используйте `steamkit::has_flag`.

## Client

`steamkit::Client` владеет SDK handler objects и callback queue:

```cpp
steamkit::Client client;

auto& user = client.user();
auto& friends = client.friends();
auto& apps = client.apps();
auto& gc = client.game_coordinator();
auto& unified = client.unified_messages();
auto& cdn = client.cdn();
```

Foundation client предоставляет `connect`, `disconnect`, методы callback queue и
typed handler accessors. Network connection behavior пока не реализован:
network APIs бросают `steamkit::NotImplemented` до появления transport parity.

## SteamID

`steamkit::SteamID` повторяет SteamID bit layout, который использует SteamKit2:

```cpp
steamkit::SteamID id(
    17825793,
    steamkit::EUniverse::Public,
    steamkit::EAccountType::Individual);

std::uint64_t raw = id.to_uint64();
std::string steam3 = id.render();
std::string steam2 = id.render(false);
```

Поддерживаемые операции:

- создание из raw `uint64_t`
- создание из account id, universe и account type
- парсинг Steam2 и Steam3 строк
- рендер Steam2 и Steam3 строк
- чтение account id, instance, universe и type
- helpers для класса аккаунта, например `is_individual_account`

`std::hash<steamkit::SteamID>` предоставлен.

## GameID

`steamkit::GameID` поддерживает app ids, game mods, shortcuts и raw ids:

```cpp
steamkit::GameID app(730);
steamkit::GameID mod(70, "cstrike");
steamkit::GameID shortcut("C:/Games/tool.exe", "Tool");
```

Поддерживаемые операции:

- чтение и запись app id
- чтение и запись game type
- чтение и запись mod id
- проверки `is_steam_app`, `is_mod`, `is_shortcut` и `is_p2p_file`

`std::hash<steamkit::GameID>` предоставлен.

## Results и async jobs

`steamkit::Result<T>` несет либо значение, либо Steam result code:

```cpp
auto ok = steamkit::Result<int>::success(42);
auto fail = steamkit::Result<int>::failure(steamkit::EResult::Fail);
```

`steamkit::AsyncJob<T>` - нативный placeholder для job-based SteamKit patterns.
Он хранит `steamkit::JobID` и будет связываться с callbacks по мере переноса
network/job layer.

## Callback Manager

Callbacks наследуются от `steamkit::CallbackMsg`. Подписка делается по типу
callback:

```cpp
struct MyCallback final : steamkit::CallbackMsg
{
    int value = 0;
};

steamkit::Client client;
steamkit::CallbackManager callbacks(client);

auto sub = callbacks.subscribe<MyCallback>([](const MyCallback& message) {
    (void)message.value;
});
```

Subscriptions - RAII objects. Уничтожение subscription или вызов `unsubscribe`
удаляет подписку.

Job-scoped subscriptions поддерживаются:

```cpp
steamkit::JobID job(123);
auto sub = callbacks.subscribe<MyCallback>(job, [](const MyCallback& message) {
    (void)message;
});
```

## Raw messages

`steamkit::ClientMsg<TBody>` и `steamkit::ClientGCMsg<TBody>` - typed raw
containers. `TBody` должен предоставлять:

```cpp
std::vector<std::byte> serialize() const;
```

Пример:

```cpp
struct EmptyBody
{
    std::vector<std::byte> serialize() const { return {}; }
};

steamkit::ClientMsg<EmptyBody> msg(steamkit::EMsg::ClientGamesPlayed);
auto bytes = msg.serialize();
```

Foundation serializer записывает message id как little-endian `uint32_t`, затем
добавляет serialized body.

## Handler accessors

Имена публичных handler accessors стабильны:

- `client.user()`
- `client.friends()`
- `client.apps()`
- `client.game_coordinator()`
- `client.unified_messages()`
- `client.cdn()`

Текущие handlers задают public extension points, но реализована только часть
поведения. Вызовы, которым нужна live CM session, бросают
`steamkit::NotImplemented`.
