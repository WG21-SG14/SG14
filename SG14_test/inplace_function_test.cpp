#include "SG14_test.h"
#include "../SG14/inplace_function.h"
#include <iostream>

namespace
{
struct Functor
{
       Functor() {}
       Functor(const Functor&) { std::cout << "copy" << std::endl; }
       Functor(Functor&&) { std::cout << "move" << std::endl; }
       void operator()()
       {
               std::cout << "functor" << std::endl;
       }
};

void Foo()
{
       std::cout << "foo" << std::endl;
}

template <typename T>
void SomeTest()
{
       T func = [] { std::cout << "lambda" << std::endl; };
       func();
       std::cout << "func = &Foo" << std::endl;
       func = &Foo;
       func();
       std::cout << "T func2 = Functor()" << std::endl;
       T func2 = Functor();
       std::cout << "func.swap(func2)" << std::endl;

       // with inplace_function, this cannot simply swap pointers
       func.swap(func2);
}

}

namespace sg14_test
{

void inplace_function_test()
{
       inplace_function<void()> func = [] { std::cout << "lambda" << std::endl; };
       func();
       func = &Foo;
       func();
       inplace_function<void()> func2 = Functor();
       func.swap(func2);

       std::cout << "\nSomeTest<std::function<void()>>" << std::endl;
       SomeTest<std::function<void()>>();

       std::cout << "\nSomeTest<inplace_function<void()>>" << std::endl;
       SomeTest<inplace_function<void()>>();
}

}
