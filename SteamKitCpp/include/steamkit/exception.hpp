#pragma once

#include <stdexcept>
#include <string>

namespace steamkit
{
class SteamKitError : public std::runtime_error
{
public:
    explicit SteamKitError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class NotImplemented : public SteamKitError
{
public:
    explicit NotImplemented(const std::string& feature)
        : SteamKitError(feature + " is not implemented in SteamKitCpp yet")
    {
    }
};
}
