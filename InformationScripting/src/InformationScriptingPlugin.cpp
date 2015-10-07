/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "InformationScriptingPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"

#include "InteractionBase/src/handlers/HSceneHandlerItem.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"

#include "commands/CScript.h"
#include "helpers/BoostPythonHelpers.h"
#include "queries/ScriptQuery.h"
#include "queries/AstQuery.h"
#include "queries/AstNameFilter.h"
#include "queries/AddASTPropertiesAsTuples.h"
#include "queries/TagQuery.h"
#include "nodes/TagExtension.h"
#include "visualization/Heatmap.h"
#include "visualization/VCommandNode.h"
#include "visualization/VCommandArgument.h"
#include "visualization/VEmptyQueryNode.h"
#include "visualization/VOperatorQueryNode.h"
#include "visualization/VErrorQueryNode.h"
#include "visualization/VUnfinishedQueryNode.h"
#include "handlers/HQuery.h"
#include "interaction/QueryOperatorDescriptorList.h"

namespace InformationScripting {

bool InformationScriptingPlugin::initialize(Core::EnvisionManager&)
{
	BoostPythonHelpers::initializeConverters();
	Interaction::HSceneHandlerItem::instance()->addCommand(new CScript());
	ScriptQuery::initPythonEnvironment();
	AstQuery::registerDefaultQueries();
	AstNameFilter::registerDefaultQueries();
	AddASTPropertiesAsTuples::registerDefaultQueries();
	TagQuery::registerDefaultQueries();
	Heatmap::registerDefaultQueries();
	TagExtension::registerExtension();
	Model::CompositeNode::registerNewExtension<TagExtension>();
	VCommandNode::setDefaultClassHandler(HQuery::instance());
	VCommandArgument::setDefaultClassHandler(HQuery::instance());
	VEmptyQueryNode::setDefaultClassHandler(HQuery::instance());
	VOperatorQueryNode::setDefaultClassHandler(HQuery::instance());
	VErrorQueryNode::setDefaultClassHandler(HQuery::instance());
	VUnfinishedQueryNode::setDefaultClassHandler(HQuery::instance());
	HQuery::initStringComponents();
	QueryOperatorDescriptorList::initializeWithDefaultOperators();
	return true;
}

void InformationScriptingPlugin::unload()
{
	ScriptQuery::unloadPythonEnvironment();
}

void InformationScriptingPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<InformationScriptingPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<InformationScriptingPlugin>::runTest(testid).printResultStatistics();
}

}
