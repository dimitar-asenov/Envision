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
#include "SelfTest/src/TestManager.h"
#include "python_bindings/DataApi.h"

#include "nodes/TagExtension.h"
#include "python_bindings/BoostPythonHelpers.h"
#include "queries/ScriptQuery.h"
#include "queries/AstQuery.h"
#include "queries/AddASTPropertiesAsTuples.h"
#include "queries/TagQuery.h"
#include "queries/VersionControlQuery.h"
#include "queries/BreakpointManager.h"
#include "queries/RuntimeQuery.h"
#include "queries/Heatmap.h"
#include "queries/Count.h"
#include "queries/Join.h"
#include "queries/Filter.h"
#include "queries/CanReach.h"
#include "queries/Yield.h"
#include "query_prompt/HQuery.h"
#include "query_prompt/visualization/VCommandNode.h"
#include "query_prompt/visualization/VCommandArgument.h"
#include "query_prompt/visualization/VOperatorQueryNode.h"
#include "query_prompt/QueryPromptMode.h"
#include "query_prompt/parsing/QueryBuilder.h"
#include "query_framework/QueryResultVisualizer.h"

#include "OOInteraction/src/string_offset_providers/StringOffsetProvider.h"

#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"

#include "InteractionBase/src/prompt/Prompt.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"

namespace InformationScripting {

bool InformationScriptingPlugin::initialize(Core::EnvisionManager&)
{
	Core::TypeRegistry::initializeNewTypes();

	BoostPythonHelpers::initializeConverters();
	initPythonEnvironment();
	AstQuery::registerDefaultQueries();
	AddASTPropertiesAsTuples::registerDefaultQueries();
	TagQuery::registerDefaultQueries();
	VersionControlQuery::registerDefaultQueries();
	Heatmap::registerDefaultQueries();
	BreakpointManager::registerDefaultQueries();
	RuntimeQuery::registerDefaultQueries();
	Count::registerDefaultQueries();
	Join::registerDefaultQueries();
	Filter::registerDefaultQueries();
	Reachable::registerDefaultQueries();
	QueryResultVisualizer::registerDefaultQueries();
	Yield::registerDefaultQueries();
	TagExtension::registerExtension();
	Model::CompositeNode::registerNewExtension<TagExtension>();
	VCommandNode::setDefaultClassHandler(HQuery::instance());
	VCommandArgument::setDefaultClassHandler(HQuery::instance());
	VOperatorQueryNode::setDefaultClassHandler(HQuery::instance());
	HQuery::initStringComponents();
	QueryBuilder::init();

	//Enable the default grid offset string provider for all query nodes:
	OOInteraction::StringOffsetProvider::allowGridBasedProvider([](Visualization::Item* item)
		{ return DCast<QueryNode>(item->node());});
	OOInteraction::StringOffsetProvider::allowGridBasedProvider([](Visualization::Item* item)
		{ return DCast<Model::TypedList<QueryNode>>(item->node());});

	// Register the query prompt
	Interaction::Prompt::registerMode<QueryPromptMode>("query");
	Interaction::Prompt::registerPromptShowShortcut(Qt::Key_Q,
		[](Visualization::Item* target){Interaction::Prompt::show("query", target);});

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
