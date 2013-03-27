#include "test.h"
#include <iostream>
//TEst comment

int NOCLASSFIELD;

namespace TestSpace{
namespace TestSpace {



class Testing {
public:
    int i;
    Testing(){}
    int myFunction(int argument1, bool argument2){
        int k = 20;
        if(k == 10)
        {
            int l = 100;
            k+=20;
        }
        else
        {
            int f=0;
            k*=0;
        }
        return k;}
};
}


namespace innerNameSpace {


class Another {
    //COMMENT IN CLAS
    bool aBoolVar;
    void aVoidFunc(){
        bool aNotherBoolVar;};
private:
    class InAnotherClass{
        int inAnotherVar;
    };

};
}
}

class OneMore{
    int oneMoreFunction(){
        int oneMoreVariable, oneMoreVar2;
        oneMoreVariable = 100 + oneMoreVar2;
        while(oneMoreVariable < 1000)
        {
            oneMoreVariable = oneMoreVariable+100;
        }
        if(oneMoreVariable <=1000)
        {
            oneMoreVariable = oneMoreVariable+50;
        }
        else if(oneMoreVariable > 1000)
        {
            oneMoreVariable = oneMoreVariable+200;
        }
        else if(oneMoreVar2 >= 1000)
        {
            oneMoreVariable = oneMoreVariable+300;
        }
        return 100;
    }
};

int Header::headerFunc()
{
    int someVarInCPP[10];
    int k;
    for(int i=0; i<100;i++)
    {
        k = i;
    }
    return 0;
}

int Header::headerFunc(int k)
{
    int anotherVar;
    return 100;
}
