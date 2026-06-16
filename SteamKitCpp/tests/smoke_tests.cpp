#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <steamkit/steamkit.hpp>

namespace
{
void expect(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

struct CallbackForTest final : steamkit::CallbackMsg
{
    explicit CallbackForTest(std::string id, steamkit::JobID job = steamkit::JobID::invalid())
        : CallbackMsg(job), unique_id(std::move(id))
    {
    }

    std::string unique_id;
};

struct TestBody
{
    std::vector<std::byte> bytes;

    [[nodiscard]] std::vector<std::byte> serialize() const
    {
        return bytes;
    }
};

void steam_id_smoke()
{
    steamkit::SteamID empty;
    expect(!empty.is_valid(), "default SteamID should be invalid");

    steamkit::SteamID clan(103582791432294076ULL);
    expect(clan.account_id() == 2772668, "clan account id should match SteamKit2");
    expect(clan.account_instance() == steamkit::SteamID::AllInstances, "clan instance should match SteamKit2");
    expect(clan.account_universe() == steamkit::EUniverse::Public, "clan universe should match SteamKit2");
    expect(clan.account_type() == steamkit::EAccountType::Clan, "clan type should match SteamKit2");

    steamkit::SteamID steam2("STEAM_0:0:4491990");
    expect(steam2.account_id() == 8983980, "Steam2 account id should parse");
    expect(steam2.render() == "[U:1:8983980]", "Steam3 render should match SteamKit2");
    expect(steam2.render(false) == "STEAM_0:0:4491990", "Steam2 render should round-trip");
}

void game_id_smoke()
{
    steamkit::GameID app(570);
    expect(app.is_steam_app(), "app GameID should be steam app");
    expect(app.app_id() == 570, "app id should match input");

    steamkit::GameID mod(420, "Research and Development");
    expect(mod.is_mod(), "mod GameID should be mod");
    expect(mod.to_uint64() == 10210309621176861092ULL, "mod GameID CRC should match SteamKit2");

    steamkit::GameID shortcut("\"C:\\Program Files (x86)\\Git\\mingw64\\bin\\wintoast.exe\"", "Git for Windows");
    expect(shortcut.to_uint64() == 12754778225939316736ULL, "shortcut GameID CRC should match SteamKit2");
}

void callback_smoke()
{
    steamkit::Client client;
    steamkit::CallbackManager manager(client);

    bool called = false;
    auto subscription = manager.subscribe<CallbackForTest>([&](const CallbackForTest& callback) {
        called = callback.unique_id == "one";
    });

    client.post_callback(std::make_shared<CallbackForTest>("one"));
    expect(manager.run_callbacks(), "callback manager should run posted callback");
    expect(called, "typed callback should be invoked");

    bool wrong_job_called = false;
    auto wrong_job_subscription = manager.subscribe<CallbackForTest>(steamkit::JobID(123), [&](const CallbackForTest&) {
        wrong_job_called = true;
    });

    client.post_callback(std::make_shared<CallbackForTest>("two", steamkit::JobID(456)));
    expect(manager.run_callbacks(), "callback with unmatched job still drains");
    expect(!wrong_job_called, "wrong job subscription should not run");
}

void message_smoke()
{
    TestBody body{{std::byte{0xCA}, std::byte{0xFE}}};

    steamkit::ClientMsg<TestBody> client_msg(steamkit::EMsg::ClientGamesPlayed, body);
    auto client_data = client_msg.serialize();
    expect(client_data.size() == 6, "client message should serialize header plus body");
    expect(client_data[0] == std::byte{0xE6} && client_data[1] == std::byte{0x02}, "client emsg should be little-endian");

    steamkit::ClientGCMsg<TestBody> gc_msg(9127, body);
    auto gc_data = gc_msg.serialize();
    expect(gc_data.size() == 6, "gc message should serialize header plus body");
    expect(gc_data[0] == std::byte{0xA7} && gc_data[1] == std::byte{0x23}, "gc emsg should be little-endian");
}
}

int main()
{
    try
    {
        steam_id_smoke();
        game_id_smoke();
        callback_smoke();
        message_smoke();
        std::cout << "SteamKitCpp smoke tests passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "SteamKitCpp smoke tests failed: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
