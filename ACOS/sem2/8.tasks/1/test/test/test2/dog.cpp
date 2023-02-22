#include "../../inter.h"
#include <iostream>

namespace test::test2
{
class Dog : inter
{
  public:
    Dog();
    int x = 0;
};

Dog::Dog()
{
    x = 34;
    std::cout << "hello I'm dog from test test2" << std::endl;
}
Dog dog;
} // namespace test::test2
