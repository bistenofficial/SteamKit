add_library(SteamKitCpp STATIC)
add_library(SteamKitCpp::steamkit ALIAS SteamKitCpp)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

target_compile_features(SteamKitCpp PUBLIC cxx_std_20)
target_include_directories(
    SteamKitCpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources(
    SteamKitCpp
    PRIVATE
        src/async_job.cpp
        src/callbacks.cpp
        src/client.cpp
        src/messages.cpp
        src/result.cpp
        src/types.cpp
)

set_target_properties(
    SteamKitCpp
    PROPERTIES
        EXPORT_NAME steamkit
        OUTPUT_NAME SteamKitCpp
)

if(STEAMKITCPP_BUILD_TESTS AND BUILD_TESTING)
    find_package(Catch2 3 CONFIG QUIET)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    find_program(POWERSHELL_EXECUTABLE NAMES powershell pwsh)

    if(Catch2_FOUND)
        add_executable(SteamKitCppTests tests/steamkitcpp_tests.cpp)
        target_link_libraries(SteamKitCppTests PRIVATE SteamKitCpp::steamkit Catch2::Catch2WithMain)

        include(Catch)
        catch_discover_tests(SteamKitCppTests)
    else()
        message(STATUS "Catch2 was not found; building dependency-free SteamKitCppSmokeTests instead.")
        add_executable(SteamKitCppSmokeTests tests/smoke_tests.cpp)
        target_link_libraries(SteamKitCppSmokeTests PRIVATE SteamKitCpp::steamkit)
        add_test(NAME SteamKitCppSmokeTests COMMAND SteamKitCppSmokeTests)
    endif()

    if(POWERSHELL_EXECUTABLE)
        add_test(
            NAME SteamKitCppSecretIgnoreTests
            COMMAND
                ${POWERSHELL_EXECUTABLE}
                -NoProfile
                -ExecutionPolicy
                Bypass
                -File
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/secret_ignore_tests.ps1
                -RepoRoot
                ${CMAKE_SOURCE_DIR}
        )

        if(Python3_Interpreter_FOUND)
            add_test(
                NAME SteamKitCppGeneratorTests
                COMMAND
                    ${POWERSHELL_EXECUTABLE}
                    -NoProfile
                    -ExecutionPolicy
                    Bypass
                    -File
                    ${CMAKE_CURRENT_SOURCE_DIR}/tests/generator_tests.ps1
                    -RepoRoot
                    ${CMAKE_SOURCE_DIR}
                    -PythonExecutable
                    ${Python3_EXECUTABLE}
            )
        endif()
    endif()
endif()

if(STEAMKITCPP_BUILD_SAMPLES)
    add_executable(SteamKitCppLevelProbeSample samples/level_probe.cpp)
    target_link_libraries(SteamKitCppLevelProbeSample PRIVATE SteamKitCpp::steamkit)
endif()

install(
    TARGETS SteamKitCpp
    EXPORT SteamKitCppTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_package_config_file(
    cmake/SteamKitCppConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SteamKitCppConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SteamKitCpp
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SteamKitCppConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/SteamKitCppConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SteamKitCppConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SteamKitCpp
)

install(
    EXPORT SteamKitCppTargets
    NAMESPACE SteamKitCpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SteamKitCpp
)
