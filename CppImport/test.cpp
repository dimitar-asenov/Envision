#include "test.h"
//#include <iostream>
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
        bool __plus;
        int __c,__lit, __lc;
        if ((__plus || __c == __lit)
            && !(__lc && __c == __lc)
            && !(__c == __lc))
            return 0;
        return k;}
    int callExpr() {
        return this->myFunction(1,true);
    }

    int someCall() {
        return myFunction(1, ~false);
    }

    int takesRef(int& ref) {
        ref = ref < 10 ? 20 : 0;
        return 0;
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
        aTestingObj.myFunction(10,true);
        bool aNotherBoolVar;}
public:
    int aPublicvar;
    int aPublicFunction(){
        return 0;
    }
    int aSwitchFunction(int k) {
        switch(k) {
        case 0:
            return 10;
        case 1==1:
            return 20;
        default:
            k = 100;
            break;
        }
        return k;
    }

private:
    class InAnotherClass{
        int inAnotherVar;
        InAnotherClass* getClass(){
            return this;
        }

        InAnotherClass* getNewInst(){
            return new InAnotherClass();
        }

        InAnotherClass* getNull(){
            return 0;
        }
        void test() {
            auto cl = new InAnotherClass();
            cl->getClass();
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
        bool same = oneMoreVariable == oneMoreVar2;
        while(oneMoreVariable < 1000)
        {
            oneMoreVariable = (oneMoreVariable+100);
            if(oneMoreVariable>10)
                continue;
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
        else if(!oneMoreVariable)
            return 0;
        return 100;
    }
};

class HeaderChild : public Header, public OneMore{
private:
    int test;
};


class Exceptions
{
public:
    int testException() {
        int value = 0;
        try
        {
            value=10;
        }
        catch (int e)
        {
            value = -1;
        }
        return value;

    }
};


class Casts {
    enum TestValues {ZERO, ONE, TWO};
public:
    int returnZero() {
        return static_cast<int>(ZERO);
    }
};

class Lambda
{
public:
    bool testLambda(int arg) {
        auto x = [](int arg){return arg>10;};
        return x;
    }
};

int Header::headerFunc()
{
    float someFloat = 13.4;
    unsigned long uLong;
    int someArray[10];
    for(int a=0;a<10;a++)
        someArray[a] = a;
    int array[] = {1,2,3};
    const char* string = "Testing";
    char someChar = 'a';
    int* newArray = new int[20];
    int k;
    for(int i=0; i<100;i++)
    {
        k = i;
        if(k>10)
            break;
    }
    auto r = k;
    return r;
}

int Header::headerFunc(int k)
{
    int anotherVar;
    anotherVar = headerFunc() + 200;
    return headerFunc();
}
