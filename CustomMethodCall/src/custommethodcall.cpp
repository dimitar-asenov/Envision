/***********************************************************************************************************************
 * custommethodcall.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "custommethodcall.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "CustomVisualization.h"

#include "items/FindMethodVis.h"
#include "items/EmptyMethodVis.h"
#include "items/InsertMethodVis.h"
#include "items/SumMethodVis.h"

#include "OOModel/headers/statements/MethodCallStatement.h"
#include "OOModel/headers/expressions/MethodCallExpression.h"
#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/Scene.h"

Q_EXPORT_PLUGIN2( custommethodcall, CustomMethodCall::CustomMethodCall )

using namespace Visualization;
using namespace OOModel;

namespace CustomMethodCall {

bool CustomMethodCall::initialize(Envision::EnvisionManager&)
{
	// Register extensions
	CustomVisualization::registerExtension();
	Method::registerNewExtension<CustomVisualization>();

	// Override existing visualization
	Scene::defaultRenderer()->registerVisualization(MethodCallStatement::typeIdStatic(), CustomVisualization::createStatement);
	Scene::defaultRenderer()->registerVisualization(MethodCallExpression::typeIdStatic(), CustomVisualization::createExpression);

	//Register custom visualizations
	CustomVisualization::registerVisualization(FindMethodVis::className(), createVisualization<FindMethodVis, MethodCallStatement>);
	CustomVisualization::registerVisualization(EmptyMethodVis::className(), createVisualization<EmptyMethodVis, MethodCallStatement>);
	CustomVisualization::registerVisualization(InsertMethodVis::className(), createVisualization<InsertMethodVis, MethodCallStatement>);
	CustomVisualization::registerVisualization(SumMethodVis::className(), createVisualization<SumMethodVis, MethodCallStatement>);

	return true;
}

void CustomMethodCall::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<CustomMethodCall>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<CustomMethodCall>::runTest(testid).printResultStatistics();
}

}
