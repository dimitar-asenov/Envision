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

#include "OOVisualizationPlugin.h"
#include "SelfTest/src/TestManager.h"

#include "allOOVisualizations.h"

#include "OOModel/src/allOOModelNodes.h"

#include "InteractionBase/src/handlers/GenericHandler.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/node_extensions/FullDetailSize.h"

#include "info/InfoMethods.h"
#include "VisualizationBase/src/nodes/InfoNode.h"
#include "VisualizationBase/src/items/ViewItem.h"

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

bool OOVisualizationPlugin::initialize(Core::EnvisionManager&)
{
	// Register extensions
	Project::registerNewExtension<Position>();
	Module::registerNewExtension<Position>();
	Class::registerNewExtension<Position>();
	Method::registerNewExtension<Position>();

	Project::registerNewExtension<FullDetailSize>();
	Module::registerNewExtension<FullDetailSize>();
	Class::registerNewExtension<FullDetailSize>();
	Method::registerNewExtension<FullDetailSize>();

	// Register visualizations
	Core::TypeRegistry::initializeNewTypes();
	Scene::defaultRenderer()->registerVisualization(StatementItemList::typeIdStatic(),
			createVisualization<VStatementItemList, StatementItemList>, VStatementItemList::typeIdStatic());

	// Use default handler:
	VOOReference::setDefaultClassHandler(Interaction::GenericHandler::instance());


	/*Scene::defaultRenderer()->registerSemanticZoomLevel("public_interface", 1);

	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose", "public_interface",
		createVisualization<VClassSzPublic, Class>, VClassSzPublic::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "public_interface",
		createVisualization<VMethodSzPublic, Method>, VMethodSzPublic::typeIdStatic());*/


	Scene::defaultRenderer()->registerSemanticZoomLevel("method_abstraction", 2);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose", "method_abstraction",
		createVisualization<VDeclarationSz, Method>, VDeclarationSz::typeIdStatic());


	Scene::defaultRenderer()->registerSemanticZoomLevel("class_method_abstraction", 3);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose",
		"class_method_abstraction", createVisualization<VDeclarationSz, Method>, VDeclarationSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose",
		"class_method_abstraction", createVisualization<VDeclarationSz, Class>, VDeclarationSz::typeIdStatic());


	Scene::defaultRenderer()->registerSemanticZoomLevel("module_class_method_abstraction", 4);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose",
		"module_class_method_abstraction", createVisualization<VDeclarationSz, Method>, VDeclarationSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose",
		"module_class_method_abstraction", createVisualization<VDeclarationSz, Class>, VDeclarationSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), "default_purpose",
		"module_class_method_abstraction", createVisualization<VDeclarationSz, Module>, VDeclarationSz::typeIdStatic());


	Scene::defaultRenderer()->registerSemanticZoomLevel("project_module_class_method_abstraction", 5);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose",
		"project_module_class_method_abstraction", createVisualization<VDeclarationSz, Method>,
																	VDeclarationSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose",
		"project_module_class_method_abstraction", createVisualization<VDeclarationSz, Class>,
																	VDeclarationSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), "default_purpose",
		"project_module_class_method_abstraction", createVisualization<VDeclarationSz, Module>,
																	VDeclarationSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), "default_purpose",
		"project_module_class_method_abstraction", createVisualization<VDeclarationSz, Project>,
																	VDeclarationSz::typeIdStatic());


	Scene::defaultRenderer()->registerSemanticZoomLevel("test", 1);

	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), "default_purpose",
		"test", createVisualization<VDeclarationConstantSz, Method>, VDeclarationConstantSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), "default_purpose",
		"test", createVisualization<VDeclarationConstantSz, Class>, VDeclarationConstantSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), "default_purpose",
		"test", createVisualization<VDeclarationConstantSz, Module>, VDeclarationConstantSz::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), "default_purpose",
		"test", createVisualization<VDeclarationConstantSz, Project>, VDeclarationConstantSz::typeIdStatic());


	int purpose = Visualization::ViewItem::publicInterfacePurpose();
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), purpose,
				createVisualization<VMethodSzPublic, Method>, VMethodSzPublic::typeIdStatic());
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), purpose,
				createVisualization<VClassSzPublic, Class>, VClassSzPublic::typeIdStatic());

	Visualization::InfoNode::registerInfoGetter("callees", InfoMethods::numberOfCallees, true, true);
	Visualization::InfoNode::registerInfoGetter("name", InfoMethods::fullName, true, true);
	Visualization::InfoNode::registerInfoGetter("usages", InfoMethods::numberOfUsages, false, true);

	return true;
}

void OOVisualizationPlugin::unload()
{
}

void OOVisualizationPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualizationPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualizationPlugin>::runTest(testid).printResultStatistics();
}

}
