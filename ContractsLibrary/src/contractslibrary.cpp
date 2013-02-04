/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * contractslibrary.cpp
 *
 *  Created on: May 11, 2012
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "contractslibrary.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "monitor/ValueAtReturnVisitor.h"
#include "monitor/ChangeMonitor.h"

#include "OOInteraction/src/handlers/HExpression.h"

#include "ModelBase/src/adapter/AdapterManager.h"

Q_EXPORT_PLUGIN2( contractslibrary, ContractsLibrary::ContractsLibrary )

namespace ContractsLibrary {

bool ContractsLibrary::initialize(Core::EnvisionManager&)
{
	ValueAtReturnVisitor::init();

	// Create a change monitor
	//auto cm = new ChangeMonitor();
	//cm->listenToModel(model);
	OOInteraction::HExpression::instance()->appendExpressionMonitor(ChangeMonitor::expressionModified);

	return true;
}

void ContractsLibrary::unload()
{
}

void ContractsLibrary::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<ContractsLibrary>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<ContractsLibrary>::runTest(testid).printResultStatistics();
}

}
