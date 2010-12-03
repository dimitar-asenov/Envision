/***********************************************************************************************************************
 * filepersistence.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "BinaryNode.h"
#include "BinaryNodeUnit.h"

#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( filepersistence, FilePersistence::FilePersistence )

namespace FilePersistence {

bool FilePersistence::initialize(Envision::EnvisionManager&)
{
	return true;
}

void FilePersistence::selfTest(QString)
{
	BinaryNode::init();
	BinaryNodeUnit::init();
	SelfTest::TestManager<FilePersistence>::runAllTests().printResultStatistics();
}

}
