# Секреты и безопасность live-аккаунтов SteamKitCpp

SteamKitCpp tests по умолчанию не должны зависеть от live Steam accounts.
Реальные credentials, `.maFile` files, cookies, refresh tokens и access tokens
нельзя коммитить.

## Игнорируемые secret patterns

Репозиторный `.gitignore` блокирует типовые local secret files:

- `*.maFile`
- `*.mafile`
- `*.secret.json`
- `*.local.json`
- `*.env`
- `.secret-ignore-probe/`
- `secrets/`

`SteamKitCpp/tests/secret_ignore_tests.ps1` проверяет эти patterns через
создание fake files и `git check-ignore`. Он не читает реальные account files.

## Политика live login

Live login tests должны быть opt-in. Они должны требовать явные local
environment variables или command-line arguments и пропускаться по умолчанию в
CI и обычных local CTest runs.

Live tests не должны:

- печатать credentials, tokens, shared secrets, identity secrets или cookies
- менять account settings
- принимать trades
- делать purchases
- менять inventory, profile или security settings
- записывать account fixtures в репозиторий

## Политика безопасных fixtures

Предпочтительные fixtures:

- static binary packet captures без account secrets
- generated fake account ids только для parser behavior
- local JSON fixtures с dummy values
- SteamKit2 test fixtures, уже пригодные для репозитория

Не используйте fixtures из реальных login sessions, если они явно не очищены и
не reviewed.

## Review checklist

Перед commit tests или samples:

- Запустить `git diff --cached --check`.
- Проверить staged changes на account ids, tokens и secret field names.
- Убедиться, что `.maFile` files ignored и не staged.
- Убедиться, что samples не делают live login по умолчанию.
- Убедиться, что любой live-test hook описан как opt-in.
