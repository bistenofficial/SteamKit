cmake_minimum_required(VERSION 3.24)

project(SteamKitNativePort VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

option(STEAMKITCPP_BUILD_TESTS "Build SteamKitCpp tests" ON)
option(STEAMKITCPP_BUILD_SAMPLES "Build SteamKitCpp samples" ON)

add_subdirectory(SteamKitCpp)
