struct Empty {};

int main(){
  int k = sizeof(int);
  int c = alignof(Empty);
  return 0;
}
