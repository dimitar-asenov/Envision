#include <iostream>

void sFunction(int arg)
{
  for(int a, b, c = arg,d; a<arg; b++)
  for(int a, b, c = arg; a<arg; b++)
    std::cout<<"print";
    for(int a, b; a<arg; b++)
    std::cout<<"print";
    for(int a; a<arg; a++)
    std::cout<<"print";
}

int main()
{
  sFunction(10);
}
