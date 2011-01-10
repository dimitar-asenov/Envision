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
#include "nodes/List.h"
#include "test_nodes/BinaryNode.h"
#include "test_nodes/BinaryWithPosition.h"
#include "test_nodes/BinaryNodeAccessUnit.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( modelbase, Model::ModelBase )

namespace Model {

Log* ModelBase::logger = NULL;

bool ModelBase::initialize(Envision::EnvisionManager&)
{
	logger = Logger::Log::getLogger("modelbase");
	Text::registerNodeType();
	Integer::registerNodeType();
	Reference::registerNodeType();
	List::registerNodeType();
	return true;
}

void ModelBase::selfTest(QString)
{
	TestNodes::BinaryNode::init();
	TestNodes::BinaryWithPosition::init();
	TestNodes::BinaryNodeAccessUnit::init();
	SelfTest::TestManager<ModelBase>::runAllTests().printResultStatistics();
}

Log* ModelBase::log()
{
	return logger;
}

}
