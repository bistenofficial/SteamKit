#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <steamkit/steamkit.hpp>

namespace
{
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
}

TEST_CASE("SteamID matches SteamKit2 bit layout and rendering")
{
    steamkit::SteamID empty;
    REQUIRE_FALSE(empty.is_valid());

    steamkit::SteamID sid(1234, steamkit::SteamID::ConsoleInstance, steamkit::EUniverse::Beta, steamkit::EAccountType::Chat);
    REQUIRE(sid.account_id() == 1234);
    REQUIRE(sid.account_instance() == steamkit::SteamID::ConsoleInstance);
    REQUIRE(sid.account_universe() == steamkit::EUniverse::Beta);
    REQUIRE(sid.account_type() == steamkit::EAccountType::Chat);

    steamkit::SteamID clan(103582791432294076ULL);
    REQUIRE(clan.account_id() == 2772668);
    REQUIRE(clan.account_instance() == steamkit::SteamID::AllInstances);
    REQUIRE(clan.account_universe() == steamkit::EUniverse::Public);
    REQUIRE(clan.account_type() == steamkit::EAccountType::Clan);

    steamkit::SteamID steam2("STEAM_0:1:4491990");
    REQUIRE(steam2.account_id() == 8983981);
    REQUIRE(steam2.account_instance() == steamkit::SteamID::DesktopInstance);
    REQUIRE(steam2.account_universe() == steamkit::EUniverse::Public);

    steamkit::SteamID steam3;
    REQUIRE(steam3.set_from_steam3_string("[A:2:165:1234]"));
    REQUIRE(steam3.render() == "[A:2:165:1234]");

    steamkit::SteamID rendered(76561197969249708ULL);
    REQUIRE(rendered.render() == "[U:1:8983980]");
    REQUIRE(rendered.render(false) == "STEAM_0:0:4491990");
}

TEST_CASE("GameID matches SteamKit2 app, mod, and shortcut ids")
{
    steamkit::GameID app(570);
    REQUIRE(app.app_id() == 570);
    REQUIRE(app.is_steam_app());

    steamkit::GameID mod(420, "Research and Development");
    REQUIRE(mod.is_mod());
    REQUIRE(mod.app_id() == 420);
    REQUIRE(mod.to_uint64() == 10210309621176861092ULL);

    steamkit::GameID hidden(215, "hidden");
    REQUIRE(hidden.to_uint64() == 9826266959967158487ULL);

    steamkit::GameID shortcut("\"C:\\Program Files (x86)\\Git\\mingw64\\bin\\wintoast.exe\"", "Git for Windows");
    REQUIRE(shortcut.is_shortcut());
    REQUIRE(shortcut.to_uint64() == 12754778225939316736ULL);
}

TEST_CASE("CallbackManager routes typed callbacks and respects JobID subscriptions")
{
    steamkit::Client client;
    steamkit::CallbackManager manager(client);

    bool did_call = false;
    auto subscription = manager.subscribe<CallbackForTest>([&](const CallbackForTest& callback) {
        did_call = callback.unique_id == "callback-1";
    });

    client.post_callback(std::make_shared<CallbackForTest>("callback-1"));

    REQUIRE(manager.run_callbacks());
    REQUIRE(did_call);

    bool wrong_job_called = false;
    auto wrong_job_subscription = manager.subscribe<CallbackForTest>(steamkit::JobID(123), [&](const CallbackForTest&) {
        wrong_job_called = true;
    });

    client.post_callback(std::make_shared<CallbackForTest>("callback-2", steamkit::JobID(456)));

    REQUIRE(manager.run_callbacks());
    REQUIRE_FALSE(wrong_job_called);
}

TEST_CASE("ClientMsg and ClientGCMsg expose raw serialized message containers")
{
    TestBody body{{std::byte{0xCA}, std::byte{0xFE}}};

    steamkit::ClientMsg<TestBody> client_msg(steamkit::EMsg::ClientGamesPlayed, body);
    auto client_data = client_msg.serialize();
    REQUIRE(client_msg.message_type() == steamkit::EMsg::ClientGamesPlayed);
    REQUIRE(client_data.size() == 6);
    REQUIRE(client_data[0] == std::byte{0xE6});
    REQUIRE(client_data[1] == std::byte{0x02});
    REQUIRE(client_data[4] == std::byte{0xCA});
    REQUIRE(client_data[5] == std::byte{0xFE});

    steamkit::ClientGCMsg<TestBody> gc_msg(9127, body);
    auto gc_data = gc_msg.serialize();
    REQUIRE(gc_msg.message_type() == 9127);
    REQUIRE(gc_data.size() == 6);
    REQUIRE(gc_data[0] == std::byte{0xA7});
    REQUIRE(gc_data[1] == std::byte{0x23});
    REQUIRE(gc_data[4] == std::byte{0xCA});
    REQUIRE(gc_data[5] == std::byte{0xFE});
}
