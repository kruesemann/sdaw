# sdaw

sdaw is a header-only library for simple mutex-protected access of data.

# Usage sample

## demo.cpp

```cpp
#include "include/shared_data_access_wrapper.h"

#include <future>
#include <iostream>
#include <mutex>
#include <shared_mutex>


struct SharedData
{
    int integer = 0;

    bool boolean = false;
};

class SharedDataOwner
{
public:

    auto get_shared_data_read_access() const
    {
        return sdaw::shared_data_read_wrapper<SharedData>(sharedData, sharedDataMutex);
    }

    auto get_shared_data_write_access()
    {
        return sdaw::shared_data_write_wrapper<SharedData>(sharedData, sharedDataMutex);
    }

private:

    SharedData sharedData;

    mutable std::shared_mutex sharedDataMutex;
};

int main()
{
    SharedDataOwner sharedDataOwner;
    auto const readFuture = std::async(std::launch::async,
                                       [&]()
                                       {
                                           for (size_t i = 0u; i < 5; ++i)
                                           {
                                               {
                                                   auto const readAccess = sharedDataOwner.get_shared_data_read_access();
                                                   std::cout << "integer: " << readAccess->integer << ", "
                                                       << "boolean: " << std::boolalpha << readAccess->boolean << '\n';
                                               }
                                               std::this_thread::sleep_for(std::chrono::seconds(1));
                                           }
                                       });
    auto const writeFuture = std::async(std::launch::async,
                                        [&]()
                                        {
                                            std::this_thread::sleep_for(std::chrono::seconds(2));
                                            auto const writeAccess = sharedDataOwner.get_shared_data_write_access();
                                            writeAccess->integer += 42;
                                            writeAccess->boolean = !writeAccess->boolean;
                                            std::cout << "shared data modified\n";
                                        });
}

```
