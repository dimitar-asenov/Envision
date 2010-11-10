/***********************************************************************************************************************
 * TestAssertions.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TESTASSERTIONS_H_
#define TESTASSERTIONS_H_

/**********************************************************************************************************************/
#define CHECK_GENERIC_EQUAL(expected, actual)																								\
	try																																					\
	{																																						\
		if ( (expected) == (actual) ) testResults.addPassed();																			\
		else																																				\
		{																																					\
			QString message = getName() + "   " + __FILE__ + ":" + QString::number(__LINE__) + "\n";							\
			message += "Actual value is different from expected";																			\
			testResults.addFailed(message);																										\
		}																																					\
	} catch (...)																																		\
	{																																						\
		testResults.addFailed(getName() + "   " + __FILE__ + ":" + QString::number(__LINE__) + "\nUncaught exception");\
	}
/**********************************************************************************************************************/

/**********************************************************************************************************************/
#define CHECK_INT_EQUAL(expected, actual)																										\
	try																																					\
	{																																						\
		if ( (expected) == (actual) ) testResults.addPassed();																			\
		else																																				\
		{																																					\
			QString message = getName() + "   " + __FILE__ + ":" + QString::number(__LINE__) + "\n";							\
			message += "Actual value is different from expected\n";																		\
			message += "Actual    " + QString::number(actual) + "\n";																	\
			message += "Expected  " + QString::number(expected);																			\
			testResults.addFailed(message);																										\
		}																																					\
	} catch (...)																																		\
	{																																						\
		testResults.addFailed(getName() + "   " + __FILE__ + ":" + QString::number(__LINE__) + "\nUncaught exception");\
	}
/**********************************************************************************************************************/

/**********************************************************************************************************************/
#define CHECK_CONDITION( condition )																											\
	try																																					\
	{																																						\
		if ( (condition) ) testResults.addPassed();																							\
		else																																				\
		{																																					\
			QString message = getName() + "   " + __FILE__ + ":" + QString::number(__LINE__) + "\n";							\
			message += "Condition is false: " #condition;																					\
			testResults.addFailed(message);																										\
		}																																					\
	} catch (...)																																		\
	{																																						\
		testResults.addFailed(getName() + "   " + __FILE__ + ":" + QString::number(__LINE__) + "\nUncaught exception");\
	}
/**********************************************************************************************************************/

#endif /* TESTASSERTIONS_H_ */
