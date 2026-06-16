# SteamKitCpp Migration Note

SteamKitCpp is intended to replace `SteamKit2.dll` for C++ consumers after full
parity is reached.

For a C++ application, link the static target:

```cmake
target_link_libraries(your_app PRIVATE SteamKitCpp::steamkit)
```

Include the public SDK header:

```cpp
#include <steamkit/steamkit.hpp>
```

The old managed dependency should remain in place until SteamKitCpp passes the
ported parity suite. After that, remove the `.NET`/`SteamKit2.dll` reference from
the application integration file and replace SteamKit2 calls with the matching
modern C++ API.
