/***********************************************************************************************************************
 * oovisualization.cpp
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "allOOVisualizations.h"

#include "OOModel/headers/allOOModelNodes.h"

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
	Scene::defaultRenderer()->registerVisualization(FormalResult::typeIdStatic(), createVisualization<VFormalResult, FormalResult>);
	Scene::defaultRenderer()->registerVisualization(ReferenceExpression::typeIdStatic(), createVisualization<VReferenceExpression, ReferenceExpression>);
	Scene::defaultRenderer()->registerVisualization(VariableAccess::typeIdStatic(), createVisualization<VVariableAccess, VariableAccess>);
	Scene::defaultRenderer()->registerVisualization(MethodCallExpression::typeIdStatic(), createVisualization<VMethodCallExpression, MethodCallExpression>);
	Scene::defaultRenderer()->registerVisualization(CastExpression::typeIdStatic(), createVisualization<VCastExpression, CastExpression>);
	Scene::defaultRenderer()->registerVisualization(NewExpression::typeIdStatic(), createVisualization<VNewExpression, NewExpression>);
	Scene::defaultRenderer()->registerVisualization(UnaryOperation::typeIdStatic(), createVisualization<VUnaryOperation, UnaryOperation>);
	Scene::defaultRenderer()->registerVisualization(ThisExpression::typeIdStatic(), createVisualization<VThisExpression, ThisExpression>);
	Scene::defaultRenderer()->registerVisualization(StringLiteral::typeIdStatic(), createVisualization<VStringLiteral, StringLiteral>);
	Scene::defaultRenderer()->registerVisualization(IntegerLiteral::typeIdStatic(), createVisualization<VIntegerLiteral, IntegerLiteral>);
	Scene::defaultRenderer()->registerVisualization(FloatLiteral::typeIdStatic(), createVisualization<VFloatLiteral, FloatLiteral>);
	Scene::defaultRenderer()->registerVisualization(CharacterLiteral::typeIdStatic(), createVisualization<VCharacterLiteral, CharacterLiteral>);
	Scene::defaultRenderer()->registerVisualization(BooleanLiteral::typeIdStatic(), createVisualization<VBooleanLiteral, BooleanLiteral>);
	Scene::defaultRenderer()->registerVisualization(NullLiteral::typeIdStatic(), createVisualization<VNullLiteral, NullLiteral>);
	Scene::defaultRenderer()->registerVisualization(MethodCallStatement::typeIdStatic(), createVisualization<VMethodCallStatement, MethodCallStatement>);
	Scene::defaultRenderer()->registerVisualization(VariableDeclaration::typeIdStatic(), createVisualization<VVariableDeclaration, VariableDeclaration>);
	Scene::defaultRenderer()->registerVisualization(NamedType::typeIdStatic(), createVisualization<VNamedType, NamedType>);
	Scene::defaultRenderer()->registerVisualization(PrimitiveType::typeIdStatic(), createVisualization<VPrimitiveType, PrimitiveType>);

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
	VFormalResult::setInteractionHandler(Interaction::GenericHandler::instance());
	ClassIcon::setInteractionHandler(Interaction::GenericHandler::instance());
	MethodIcon::setInteractionHandler(Interaction::GenericHandler::instance());
	VReferenceExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VVariableAccess::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethodCallExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VThisExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VCastExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VNewExpression::setInteractionHandler(Interaction::GenericHandler::instance());
	VUnaryOperation::setInteractionHandler(Interaction::GenericHandler::instance());
	VStringLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VIntegerLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VFloatLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VCharacterLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VBooleanLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VNullLiteral::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethodCallStatement::setInteractionHandler(Interaction::GenericHandler::instance());
	VVariableDeclaration::setInteractionHandler(Interaction::GenericHandler::instance());

	return true;
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}
