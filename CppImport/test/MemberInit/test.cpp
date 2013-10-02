class Pair {
  int a,b;
public:
  Pair(int f,int s){
    a=f;
    b=s;
  }  
};

template <class T> class A : public Pair {
  A(int f, int s) : Pair(f,s){}
};
  
