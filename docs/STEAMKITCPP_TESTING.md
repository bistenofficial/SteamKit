# Тестирование SteamKitCpp

SteamKitCpp использует native C++ tests, PowerShell probes и существующие C#
SteamKit2 tests как oracle.

## Native tests

vcpkg preset собирает Catch2 tests, когда доступен `catch2`:

```powershell
$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
cmake --preset windows-x64-release-vcpkg
cmake --build --preset windows-x64-release-vcpkg
ctest --preset windows-x64-release-vcpkg
```

Текущее Catch2 coverage:

- SteamID bit layout и rendering
- GameID app, mod и shortcut ids
- CallbackManager typed routing и JobID filtering
- raw ClientMsg и ClientGCMsg serialization
- secret ignore safety probe

## Smoke tests

Non-vcpkg preset намеренно работает без Catch2:

```powershell
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release
ctest --preset windows-x64-release
```

Когда Catch2 не найден, CMake собирает `SteamKitCppSmokeTests`. Эти tests
покрывают критичное foundation behavior без external test framework.

## Generator tests

Generator probe можно запускать напрямую, когда доступен Python:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File SteamKitCpp\tests\generator_tests.ps1 `
  -RepoRoot (Get-Location).Path `
  -PythonExecutable C:\Path\To\python.exe
```

Это проверяет, что SteamLanguage enum generation работает без записи generated
headers в репозиторий.

## Managed oracle tests

Managed test suite должна оставаться зеленой, пока строится native parity:

```powershell
dotnet test SteamKit2\Tests\Tests.csproj -c Release --verbosity minimal
```

Используйте эти tests как reference behavior при переносе нового layer. Native
test должен добавляться до или вместе с C++ implementation.

## Golden parity tests

Для packet, binary IO, manifest и message layers предпочтительны golden parity
tests:

1. Использовать существующие binary fixtures из SteamKit2 tests.
2. Десериализовать через SteamKit2 как oracle.
3. Десериализовать через SteamKitCpp.
4. Сравнить stable fields и serialized output.
5. Держать fixtures deterministic и без live account secrets.

## Обязательные проверки перед push

Для documentation-only changes достаточно минимальной проверки:

```powershell
git diff --check
```

После изменений SDK или CMake запускайте полный foundation check:

```powershell
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release
ctest --preset windows-x64-release

$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
cmake --preset windows-x64-release-vcpkg
cmake --build --preset windows-x64-release-vcpkg
ctest --preset windows-x64-release-vcpkg

dotnet test SteamKit2\Tests\Tests.csproj -c Release --verbosity minimal
```
