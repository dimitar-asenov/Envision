#include <typeinfo>

struct Empty {};

int main(){
  int k = sizeof(int);
  int c = alignof(Empty);
  typeid(int);
  return 0;
}
