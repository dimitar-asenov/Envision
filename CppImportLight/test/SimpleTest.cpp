#include "../src/CppImportLightPlugin.h"
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

namespace CppImportLight {

class SimpleTest :
public SelfTest::Test<CppImportLightPlugin, SimpleTest>
{ public: void test()
{
	CHECK_INT_EQUAL(1, 1);
}};

}
