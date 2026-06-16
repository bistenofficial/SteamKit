# SteamKitCpp

SteamKitCpp is the native C++20 port of SteamKit. The first supported target is
Windows x64, built as a static library with public headers.

## Build

```powershell
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release
ctest --preset windows-x64-release
```

The build is designed for CMake, Visual Studio 2022, and vcpkg manifest mode.
For vcpkg-backed dependency resolution, set `VCPKG_ROOT` and use the `*-vcpkg`
presets:

```powershell
$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
cmake --preset windows-x64-release-vcpkg
cmake --build --preset windows-x64-release-vcpkg
ctest --preset windows-x64-release-vcpkg
```

Without Catch2 installed, the default presets fall back to a dependency-free
smoke suite.

Installed consumers can use the exported package target:

```cmake
find_package(SteamKitCpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE SteamKitCpp::steamkit)
```

## Current Layer

This foundation layer includes:

- `steamkit::SteamID`, `steamkit::GameID`, `steamkit::JobID`
- `steamkit::Client`, `steamkit::Configuration`, `steamkit::CallbackManager`
- typed callback subscriptions and RAII unsubscription
- raw `ClientMsg<TBody>` and `ClientGCMsg<TBody>` containers
- public handler accessors for `user`, `friends`, `apps`, `game_coordinator`,
  `unified_messages`, and `cdn`

Network logon, CM transport, protobuf service wrappers, CDN downloads, and full
Game Coordinator behavior are intentionally explicit `NotImplemented` failures
until their parity tests are ported.

## Code Generation

`tools/generate_steamlang_cpp.py` is the native SteamLanguage entrypoint for
generated enum headers. It accepts `--namespace` for public generated protocol
namespaces. Full message class generation should extend this path rather than
copying C# generated files.

## Live Account Safety

Do not commit Steam credentials or `.maFile` files. The repo-level `.gitignore`
blocks common live-test secret names, and `SteamKitCppSecretIgnoreTests` verifies
that those patterns stay active without reading real account files.
