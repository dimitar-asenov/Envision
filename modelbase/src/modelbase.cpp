/***********************************************************************************************************************
 * modelbase.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "selftest/headers/SelfTestSuite.h"

#include "nodes/Text.h"
#include "BinaryNode.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( modelbase, Model::ModelBase )

namespace Model {

Log* ModelBase::logger = NULL;

bool ModelBase::initialize(Envision::EnvisionManager&)
{
	logger = Logger::Log::getLogger("modelbase");
	Text::registerNodeConstructors();
	return true;
}

void ModelBase::selfTest(QString)
{
	BinaryNode::init();
	SelfTest::TestManager<ModelBase>::runAllTests().printResultStatistics();
}

Log* ModelBase::log()
{
	return logger;
}

}
