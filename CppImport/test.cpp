#include "test.h"
//TEst comment

int NOCLASSFIELD;

class Testing {
public:
    int i;
    Testing(){}
    int myFunction(int argument1, bool argument2){
        int k;
        if(10)
        {
            k=20;
        }
        else
        {
            k=0;
        }
        return 0;}
};


class Another {
    //COMMENT IN CLAS
    bool aBoolVar;
    void aVoidFunc(){
        bool aNotherBoolVar;};
};


class OneMore{
    int oneMoreFunction(){
        int oneMoreVariable, oneMoreVar2;
        return 100;
    }
};

int Header::headerFunc()
{
    int someVarInCPP;
    return 0;
}

int Header::headerFunc(int k)
{
    int anotherVar;
    return 100;
}
