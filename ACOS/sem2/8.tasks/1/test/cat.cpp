#include "inter.h"
#include <iostream>

class Cat : inter {
 public:
  Cat() {
    x = 34;
    std::cout << "hello I'm cat" << std::endl;
  }
  /*
  virtual void foo() {
    std::cout << "foo in cat" << std::endl;
  }
  */
  int x = 10;
};

Cat cat;
