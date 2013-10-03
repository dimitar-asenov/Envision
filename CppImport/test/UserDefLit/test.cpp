int operator"" _toInt(long double n) {
  return int(n); 
}

int main(){
  int m = 5.0_toInt;
} 
