#include "1_f.hpp"
#include "test/inter.h"

template <class T>
class Class: public AbstractClass {
public:
    T* newInstance(); 
};

template <class T>
T* Class<T>::newInstance() {
    size_t classSize = sizeof(T);
    void* rawPtr = newInstanceWithSize(classSize);
    return reinterpret_cast<T*>(rawPtr);
}

int main()
{
    ClassLoader l;
    auto ptr = static_cast<Class<inter>*>(l.loadClass("test::test2::Dog"))
                   ->newInstance();
    ptr->foo();
    ptr = static_cast<Class<inter>*>(l.loadClass("Cat"))
                   ->newInstance();
    ptr->foo();
}
