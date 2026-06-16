# Сборка и тесты SteamKitCpp

SteamKitCpp использует CMake presets из корня репозитория. Команды ниже
предполагают, что текущая директория - корень репозитория.

## Требования

- Windows x64
- Visual Studio 2022 или Visual Studio 2022 Build Tools
- CMake 3.24 или новее
- vcpkg для dependency-backed builds
- .NET SDK 10.0 или новее для managed SteamKit2 oracle tests

Можно использовать CMake, который поставляется с Visual Studio:

```powershell
& 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe' --version
```

## Presets

В репозитории определены четыре configure presets:

- `windows-x64-release`
- `windows-x64-debug`
- `windows-x64-release-vcpkg`
- `windows-x64-debug-vcpkg`

Non-vcpkg presets собирают static library, sample и dependency-free smoke tests.
Если Catch2 не найден, CMake печатает status message и использует
`SteamKitCppSmokeTests`.

vcpkg presets используют:

- `VCPKG_MANIFEST_DIR=${sourceDir}/SteamKitCpp`
- `VCPKG_INSTALLED_DIR=${sourceDir}/out/vcpkg_installed`
- `VCPKG_TARGET_TRIPLET=x64-windows-static`
- `VCPKG_HOST_TRIPLET=x64-windows`
- static MSVC runtime для совместимости SDK и Catch2

## Сборка без vcpkg

```powershell
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release
ctest --preset windows-x64-release
```

Ожидаемое test coverage:

- dependency-free SDK smoke tests
- secret ignore safety test

## Сборка через vcpkg

Сначала задайте `VCPKG_ROOT`:

```powershell
$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
cmake --preset windows-x64-release-vcpkg
cmake --build --preset windows-x64-release-vcpkg
ctest --preset windows-x64-release-vcpkg
```

Ожидаемое test coverage:

- Catch2 SteamKitCpp tests
- secret ignore safety test

Первый vcpkg run может занять много времени, потому что static dependencies
собираются локально. Последующие запуски должны переиспользовать
`out/vcpkg_installed`.

## Установка package

Установить SDK из уже сконфигурированного build tree:

```powershell
cmake --install out/build/windows-x64-release --config Release --prefix C:\SteamKitCpp
```

Install содержит:

- `lib/SteamKitCpp.lib`
- `include/steamkit/*.hpp`
- `lib/cmake/SteamKitCpp/SteamKitCppConfig.cmake`
- `lib/cmake/SteamKitCpp/SteamKitCppTargets.cmake`

## Использование установленного package

```cmake
cmake_minimum_required(VERSION 3.24)
project(MySteamClient LANGUAGES CXX)

find_package(SteamKitCpp CONFIG REQUIRED)

add_executable(my_client main.cpp)
target_link_libraries(my_client PRIVATE SteamKitCpp::steamkit)
```

Конфигурация consumer-проекта:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:\SteamKitCpp
cmake --build build --config Release
```

## Managed oracle tests

Пока строится native parity, старая managed test suite должна оставаться зеленой:

```powershell
dotnet test SteamKit2\Tests\Tests.csproj -c Release --verbosity minimal
```

Текущий C++ foundation не заменяет эти тесты. Они остаются reference behavior,
пока native parity suite не покроет тот же surface.
