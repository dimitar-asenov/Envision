/***********************************************************************************************************************
 * oovisualization.cpp
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "vis/top_level/VProject.h"
#include "vis/top_level/VLibrary.h"
#include "vis/top_level/VModule.h"
#include "vis/top_level/VClass.h"
#include "vis/top_level/VMethod.h"
#include "vis/elements/VField.h"
#include "vis/elements/VFormalArgument.h"

#include "icons/ClassIcon.h"
#include "icons/MethodIcon.h"

#include "expressions/VVariableAccess.h"
#include "expressions/VReferenceExpression.h"

#include "literals/VStringLiteral.h"

#include "statements/VMethodCallStatement.h"

#include "types/VNamedType.h"

#include "OOModel/headers/Project.h"
#include "OOModel/headers/Library.h"
#include "OOModel/headers/Module.h"
#include "OOModel/headers/Class.h"
#include "OOModel/headers/Method.h"
#include "OOModel/headers/Field.h"
#include "OOModel/headers/FormalArgument.h"

#include "OOModel/headers/expressions/ReferenceExpression.h"
#include "OOModel/headers/expressions/VariableAccess.h"
#include "OOModel/headers/expressions/StringLiteral.h"

#include "OOModel/headers/statements/MethodCallStatement.h"

#include "OOModel/headers/types/NamedType.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"
#include "InteractionBase/headers/handlers/HText.h"

Q_EXPORT_PLUGIN2( oovisualization, OOVisualization::OOVisualization )

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

bool OOVisualization::initialize(Envision::EnvisionManager&)
{
	// Register extensions
	Project::registerNewExtension<Position>();
	Module::registerNewExtension<Position>();
	Class::registerNewExtension<Position>();
	Method::registerNewExtension<Position>();

	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), createVisualization<VProject, Project>);
	Scene::defaultRenderer()->registerVisualization(Library::typeIdStatic(), createVisualization<VLibrary, Library>);
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), createVisualization<VModule, Module>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), createVisualization<VClass, Class>);
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), createVisualization<VMethod, Method>);
	Scene::defaultRenderer()->registerVisualization(Field::typeIdStatic(), createVisualization<VField, Field>);
	Scene::defaultRenderer()->registerVisualization(FormalArgument::typeIdStatic(), createVisualization<VFormalArgument, FormalArgument>);
	Scene::defaultRenderer()->registerVisualization(ReferenceExpression::typeIdStatic(), createVisualization<VReferenceExpression, ReferenceExpression>);
	Scene::defaultRenderer()->registerVisualization(VariableAccess::typeIdStatic(), createVisualization<VVariableAccess, VariableAccess>);
	Scene::defaultRenderer()->registerVisualization(StringLiteral::typeIdStatic(), createVisualization<VStringLiteral, StringLiteral>);
	Scene::defaultRenderer()->registerVisualization(MethodCallStatement::typeIdStatic(), createVisualization<VMethodCallStatement, MethodCallStatement>);
	Scene::defaultRenderer()->registerVisualization(NamedType::typeIdStatic(), createVisualization<VNamedType, NamedType>);

	// Register handlers
	// TODO: move this to a better place i.e. OOInteraction Plugin
	// TODO: when you do that remove the dependency in the .plugin meta file
	VProject::setInteractionHandler(Interaction::GenericHandler::instance());
	VLibrary::setInteractionHandler(Interaction::GenericHandler::instance());
	VModule::setInteractionHandler(Interaction::GenericHandler::instance());
	VClass::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethod::setInteractionHandler(Interaction::GenericHandler::instance());
	VField::setInteractionHandler(Interaction::GenericHandler::instance());
	VFormalArgument::setInteractionHandler(Interaction::GenericHandler::instance());
	ClassIcon::setInteractionHandler(Interaction::GenericHandler::instance());
	MethodIcon::setInteractionHandler(Interaction::GenericHandler::instance());
	VReferenceExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VVariableAccess::setInteractionHandler(Interaction::GenericHandler::instance());
	VStringLiteral::setInteractionHandler(Interaction::HText::instance());
	VMethodCallStatement::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}
