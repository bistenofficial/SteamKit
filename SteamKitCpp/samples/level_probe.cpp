#include <iostream>
#include <steamkit/steamkit.hpp>

int main()
{
    steamkit::Client client;

    std::cout << "SteamKitCpp sample created client with identifier: "
              << client.identifier() << '\n';
    std::cout << "Network logon is intentionally not implemented in the foundation layer yet.\n";

    return 0;
}
