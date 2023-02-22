#include <iostream>

struct inter {
    virtual void foo()
    {
        std::cout << "inter" << std::endl;
    }
};
