/***********************************************************************************************************************
 * oomodel.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "common/Visibility.h"

#include "Project.h"
#include "Module.h"
#include "Class.h"
#include "Method.h"
#include "MethodItem.h"
#include "Statement.h"

Q_EXPORT_PLUGIN2( oomodel, OOModel::OOModel )

namespace OOModel {

bool OOModel::initialize(Envision::EnvisionManager&)
{
	// Initialize common nodes
	Visibility::registerNodeType();

	// Initialize TypedList instantiations
	Model::TypedList<Project>::registerNodeType();
	Model::TypedList<Module>::registerNodeType();
	Model::TypedList<Class>::registerNodeType();
	Model::TypedList<Method>::registerNodeType();
	Model::TypedList<MethodItem>::registerNodeType();
	Model::TypedList<Statement>::registerNodeType();

	Project::init();
	Module::init();
	Class::init();
	Method::init();
	MethodItem::init();
	Statement::init();

	return true;
}

void OOModel::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOModel>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOModel>::runTest(testid).printResultStatistics();
}

}
