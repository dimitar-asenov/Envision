/***********************************************************************************************************************
 * modelbase.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "nodes/Text.h"
#include "nodes/Integer.h"
#include "nodes/Reference.h"
#include "BinaryNode.h"
#include "BinaryWithPosition.h"
#include "BinaryNodeUnit.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( modelbase, Model::ModelBase )

namespace Model {

Log* ModelBase::logger = NULL;

bool ModelBase::initialize(Envision::EnvisionManager&)
{
	logger = Logger::Log::getLogger("modelbase");
	Text::registerNodeConstructors();
	Integer::registerNodeConstructors();
	Reference::registerNodeConstructors();
	return true;
}

void ModelBase::selfTest(QString)
{
	BinaryNode::init();
	BinaryWithPosition::init();
	BinaryNodeUnit::init();
	SelfTest::TestManager<ModelBase>::runAllTests().printResultStatistics();
}

Log* ModelBase::log()
{
	return logger;
}

}
