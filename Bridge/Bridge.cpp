#include <iostream>
#if defined(_WIN32)
    #define BRIDGE_API __declspec(dllexport)
#else
    #define BRIDGE_API
#endif
namespace Bridge { BRIDGE_API void Init() { std::cout << "Bridge Init"; } }
