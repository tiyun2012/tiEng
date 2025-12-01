#include <iostream>
#if defined(_WIN32)
    #define BACKEND_API __declspec(dllexport)
#else
    #define BACKEND_API
#endif
namespace Backend { BACKEND_API void Init() { std::cout << "Engine Init"; } }
