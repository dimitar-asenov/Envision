#include "../src/PLUGINNAMEPlugin.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

namespace NAMESPACE {

class SimpleTest :
public SelfTest::Test<PLUGINNAMEPlugin, SimpleTest>
{ public: void test()
{
	CHECK_INT_EQUAL(1, 1);
}};

}
