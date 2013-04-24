#include "test.h"
#include <iostream>
//TEst comment

int NOCLASSFIELD;

void globalMethod(){
    int aVarInGlobalMethod = 100;
}

enum SomeNameEnum {FIRST};

namespace TestSpace{
namespace TestSpace {

int NameSpaceMethod(){
    return 100;
}

enum SomeNameEnum {FIRST=10,SECOND};

class Testing {
public:
    int i;
    Testing(){}
    enum AnEnum {FIRST,SECOND=(100+25),THIRD};
    int myFunction(int argument1, bool argument2){
        enum FuncEnum {LOL,LOL1};
        int k = 20;
        if(k && argument2)
        {
            int l = 100;
            l = k | 10;
            k+=20;
        }
        else
        {
            int f=0;
            k*=0;
        }
        return k;}
    int callExpr() {
        return myFunction(1,true);
    }
};
}

namespace TestSpace{
namespace innerNameSpace {


class Another {
    //COMMENT IN CLAS
    Testing aTestingObj;
    bool aBoolVar;
    void aVoidFunc(){
        bool aNotherBoolVar;};
public:
    int aPublicvar;
    int aPublicFunction(){
        return 0;
    }

private:
    class InAnotherClass{
        int inAnotherVar;
        InAnotherClass* getClass(){
            return this;
        }
    };

};
}
}
}

class OneMore{
    int oneMoreFunction(){
        int oneMoreVariable, oneMoreVar2;
        oneMoreVariable = 100 + oneMoreVar2;
        while(oneMoreVariable < 1000)
        {
            oneMoreVariable = (oneMoreVariable+100);
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

class HeaderChild : public Header, public OneMore{
private:
    int test;
};

int Header::headerFunc()
{
    int someVarInCPP[10];
    int array[] = {1,2,3};
    int k;
    for(int i=0; i<100;i++)
    {
        k = i;
        if(k>10)
            break;
    }
    return 0;
}

int Header::headerFunc(int k)
{
    int anotherVar;
    anotherVar = headerFunc() + 200;
    return headerFunc();
}
