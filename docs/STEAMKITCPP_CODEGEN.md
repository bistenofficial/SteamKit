# Генерация кода SteamKitCpp

Генерация кода в SteamKitCpp должна использовать исходные protocol definitions.
Нельзя копировать или вручную переводить generated C# output.

## SteamLanguage enums

Текущий generator entrypoint:

```powershell
python SteamKitCpp\tools\generate_steamlang_cpp.py `
  --namespace steamkit::protocol `
  --output out\generated\steam_enums.hpp `
  Resources\SteamLanguage\eresult.steamd `
  Resources\SteamLanguage\emsg.steamd
```

Генератор:

- парсит `.steamd` enum blocks
- генерирует `enum class` declarations на базе `std::uint32_t`
- поддерживает настраиваемый C++ namespace через `--namespace`
- валидирует namespace syntax перед записью header

Генератор намеренно маленький. Structured message generation должен расширять
тот же parser boundary, а не добавлять случайные ad hoc scripts.

## Protobuf generation

Будущая protobuf-поддержка должна генерировать C++ из `.proto` files в protobuf
resource tree. Целевая зависимость - vcpkg `protobuf`.

Ожидаемое направление:

1. Найти `.proto` inputs из Steam protobuf resource submodule.
2. Вызывать `protoc` во время configure или как generated-source build step.
3. Класть generated C++ outputs в build directory.
4. Добавить generated include directories к internal SDK targets.
5. Экспортировать через public headers только целевые generated namespaces.
6. Добавить golden parity tests перед тем, как считать generated types stable.

## Тестирование codegen

`SteamKitCpp/tests/generator_tests.ps1` - generator probe. Он создает временный
header, проверяет requested namespace, сверяет известные enum values и удаляет
temporary directory.

Прямой запуск:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File SteamKitCpp\tests\generator_tests.ps1 `
  -RepoRoot (Get-Location).Path `
  -PythonExecutable C:\Path\To\python.exe
```

Если CMake находит Python3 interpreter, этот тест также может попасть в CTest.

## Правила для generated files

- Generated source не должен коммититься, если он не является намеренной частью
  public SDK distribution.
- Generated files должны воспроизводиться из source protocol definitions.
- Generated code не должен содержать live account data, local paths или machine
  secrets.
- Любой generated public namespace нужно документировать в описании API после
  экспорта для consumers.
