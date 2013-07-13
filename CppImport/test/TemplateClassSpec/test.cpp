#include <iostream>

template<class T = float, int i = 5> class A
{
   public:
      A();
      int value;
};

template<> class A<> { public: A(); };
template<> class A<double, 10> { public: A(); };

template<> class A<double> {public: A(); int value; };

template<class T, int i> A<T, i>::A() : value(i) {
   std::cout << "Primary template, "
        << "non-type argument is " << std::endl;
}

A<>::A() {
   std::cout << "Explicit specialization "
        << "default arguments" << std::endl;
}

A<double, 10>::A() {
   std::cout << "Explicit specialization "
        << "<double, 10>" << std::endl;
}

A<double>::A() {
  std::cout << "Partial specialization " 
  << "<double>" << std::endl;
}

int main() {
   A<int,5> x;
   A<> y;
   A<double, 10> z;
   A<double> k;
   return 0;
}
