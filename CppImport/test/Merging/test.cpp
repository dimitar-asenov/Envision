#include <iostream>

// // forward declaration
// class MyClass;
// 
// // declaration
// class MyClass{
//   MyClass();
// };
// 
// // definition
// MyClass::MyClass(){
//   std::cout << "definition";
// }

// declaration
int func(int);

// re - declaration
int func(int i);

// definition
int func(int k){
  std::cout << k;
  return k;
}