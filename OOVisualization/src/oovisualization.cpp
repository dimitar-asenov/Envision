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

#include "oovisualization.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "allOOVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"

#include <semantic_zoom/VClassSzPublic.h>
#include <semantic_zoom/VDeclarationSz.h>
#include <semantic_zoom/VMethodSzPublic.h>
#include <semantic_zoom/VModuleSz.h>

Q_EXPORT_PLUGIN2( oovisualization, OOVisualization::OOVisualization )

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

Core::InitializationRegistry& itemTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

bool OOVisualization::initialize(Core::EnvisionManager&)
{
	// Register extensions
	Project::registerNewExtension<Position>();
	Module::registerNewExtension<Position>();
	Class::registerNewExtension<Position>();
	Method::registerNewExtension<Position>();

	// Register visualizations
	itemTypeInitializationRegistry().initializeAll();
	Scene::defaultRenderer()->registerVisualization(StatementItemList::typeIdStatic(),
			createVisualization<VStatementItemList, StatementItemList>);


	Scene::defaultRenderer()->registerSemanticZoomLevel("public_interface_zoom_level", 1);

	// Register semantic zoom: public interface visualizations
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose", "public_interface_zoom_level",
			createVisualization<VClassSzPublic, Class>);
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "public_interface_zoom_level",
			createVisualization<VMethodSzPublic, Method>);


	Scene::defaultRenderer()->registerSemanticZoomLevel("high_level_zoom_level", 2);

	// Register semantic zoom: high level visualizations
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), "default_purpose", "high_level_zoom_level",
			createVisualization<VModuleSz, Module>);


	Scene::defaultRenderer()->registerSemanticZoomLevel("declaration_test1", 3);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "declaration_test1",
			createVisualization<VDeclarationSz, Method>);


	Scene::defaultRenderer()->registerSemanticZoomLevel("declaration_test2", 4);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "declaration_test2",
			createVisualization<VDeclarationSz, Method>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose", "declaration_test2",
			createVisualization<VDeclarationSz, Class>);


	Scene::defaultRenderer()->registerSemanticZoomLevel("declaration_test3", 5);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "declaration_test3",
			createVisualization<VDeclarationSz, Method>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose", "declaration_test3",
			createVisualization<VDeclarationSz, Class>);
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), "default_purpose", "declaration_test3",
			createVisualization<VDeclarationSz, Module>);


	Scene::defaultRenderer()->registerSemanticZoomLevel("declaration_test4", 6);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "declaration_test4",
			createVisualization<VDeclarationSz, Method>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose", "declaration_test4",
			createVisualization<VDeclarationSz, Class>);
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), "default_purpose", "declaration_test4",
			createVisualization<VDeclarationSz, Module>);
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), "default_purpose", "declaration_test4",
			createVisualization<VDeclarationSz, Project>);

	return true;
}

void OOVisualization::unload()
{
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}
