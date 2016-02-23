/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#pragma once

#include "selftest_api.h"

/**
 * The Q_ASSERT(initTrigger) is needed to make sure that the test is actually initialized.
 */
#define CHECK_GENERIC_EQUAL(expected, actual)																								\
	try																																					\
	{																																						\
		Q_ASSERT(initTrigger);																														\
		if ( (expected) == (actual) ) testResults.addPassedCheck();																		\
		else																																				\
		{																																					\
			allChecksPassedFlag = false;																											\
			QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";							\
			message += "Actual value is different from expected";																			\
			testResults.addFailedCheck(message);																								\
		}																																					\
	} catch (...)																																		\
	{																																						\
		allChecksPassedFlag = false;																												\
		testResults.addFailedCheck(getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + " Uncaught exception");	\
		throw;																																			\
	}

/**
 * The Q_ASSERT(initTrigger) is needed to make sure that the test is actually initialized.
 */
#define CHECK_INT_EQUAL(expected, actual)																										\
	try																																					\
	{																																						\
		Q_ASSERT(initTrigger);																														\
		if ( (expected) == (actual) ) testResults.addPassedCheck();																		\
		else																																				\
		{																																					\
			allChecksPassedFlag = false;																											\
			QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";							\
			message += "Actual value (" + QString::number(actual);																		\
			message += ") is different from expected ("+QString::number(expected)+")";												\
			testResults.addFailedCheck(message);																								\
		}																																					\
	} catch (...)																																		\
	{																																						\
		allChecksPassedFlag = false;																												\
		testResults.addFailedCheck(getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + " Uncaught exception");	\
		throw;																																			\
	}

/**
 * The Q_ASSERT(initTrigger) is needed to make sure that the test is actually initialized.
 */
#define CHECK_STR_EQUAL(expected, actual)																										\
	try																																					\
	{																																						\
		Q_ASSERT(initTrigger);																														\
		if ( (expected) == (actual) ) testResults.addPassedCheck();																		\
		else																																				\
		{																																					\
			allChecksPassedFlag = false;																											\
			QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";							\
			message += "Actual value (" + QString{actual} +") is different from expected ("+QString{expected}+")";		\
			testResults.addFailedCheck(message);																								\
		}																																					\
	} catch (...)																																		\
	{																																						\
		allChecksPassedFlag = false;																												\
		testResults.addFailedCheck(getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + " Uncaught exception");	\
		throw;																																			\
	}

/**
 * The Q_ASSERT(initTrigger) is needed to make sure that the test is actually initialized.
 */
#define CHECK_TEXT_FILES_EQUAL(expectedFileName, actualFileName)																		\
	try																																					\
	{																																						\
		Q_ASSERT(initTrigger);																														\
		QFile exp{expectedFileName};																												\
		QFile act{actualFileName};																													\
		if (exp.open(QIODevice::ReadOnly | QIODevice::Text))																				\
		{																																					\
			if (act.open(QIODevice::ReadOnly | QIODevice::Text))																			\
			{																																				\
				QTextStream inexp{&exp};																											\
				QString strexp = inexp.readAll();																								\
																																							\
				QTextStream inact{&act};																											\
				QString stract = inact.readAll();																								\
																																							\
				if (strexp == stract) testResults.addPassedCheck();																		\
				else																																		\
				{																																			\
					allChecksPassedFlag = false;																									\
					QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";					\
					message += "File " + QString{actualFileName} + " differs from the expected output.";						\
					testResults.addFailedCheck(message);																						\
				}																																			\
			}																																				\
			else																																			\
			{																																				\
				allChecksPassedFlag = false;																										\
				QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";						\
				message += "Could not open file " + QString{actualFileName} + " for reading.";									\
				testResults.addFailedCheck(message);																							\
			}																																				\
		}																																					\
		else																																				\
		{																																					\
			allChecksPassedFlag = false;																											\
			QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";							\
			message += "Could not open file " + QString{expectedFileName} + " for reading.";										\
			testResults.addFailedCheck(message);																								\
		}																																					\
	} catch (...)																																		\
	{																																						\
		allChecksPassedFlag = false;																												\
		testResults.addFailedCheck(getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + " Uncaught exception");	\
		throw;																																			\
	}

/**
 * The Q_ASSERT(initTrigger) is needed to make sure that the test is actually initialized.
 */
#define CHECK_CONDITION( condition )																											\
	try																																					\
	{																																						\
		Q_ASSERT(initTrigger);																														\
		if ( (condition) ) testResults.addPassedCheck();																					\
		else																																				\
		{																																					\
			allChecksPassedFlag = false;																											\
			QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";							\
			message += "Condition is false: " #condition;																					\
			testResults.addFailedCheck(message);																								\
		}																																					\
	} catch (...)																																		\
	{																																						\
		allChecksPassedFlag = false;																												\
		testResults.addFailedCheck(getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + " Uncaught exception");	\
		throw;																																			\
	}

/**
 * The Q_ASSERT(initTrigger) is needed to make sure that the test is actually initialized.
 */
#define CHECK_FOR_EXCEPTION( exception, statement )																						\
{																																							\
	Q_ASSERT(initTrigger);																															\
	bool oldAssertOnThrow = exception::assertOnThrow();																					\
	exception::assertOnThrow() = false;																											\
	try																																					\
	{																																						\
		statement;																																		\
		exception::assertOnThrow() = oldAssertOnThrow;																						\
		allChecksPassedFlag = false;																												\
		QString message = getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + "\t";								\
		message += "Exception " #exception " was expected but did not occur";														\
		testResults.addFailedCheck(message);																									\
	} catch (exception &e)																															\
	{																																						\
		exception::assertOnThrow() = oldAssertOnThrow;																						\
		testResults.addPassedCheck();																												\
	} catch (...)																																		\
	{																																						\
		exception::assertOnThrow() = oldAssertOnThrow;																						\
		allChecksPassedFlag = false;																												\
		testResults.addFailedCheck(getName() + "\t" + __FILE__ + ":" + QString::number(__LINE__) + " Uncaught exception");	\
		throw;																																			\
	}																																						\
}
