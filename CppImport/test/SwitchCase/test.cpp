#include <iostream>

void sFunction(int arg)
{
 switch(arg)
 {
   std::cout << "pre amble";
   case 0:
     return;
   case 1:
     std::cout << "1" << std::endl;
     break;
   case 2:
     std::cout << "2";
     std::cout << std::endl;
   case 3:
     std::cout << "3";
     std::cout << std::endl;
     break;
   default:
     std::cout << "default";
     std::cout << std::endl;
     break;
   return;
 }
}

int main()
{
  sFunction(10);
}
