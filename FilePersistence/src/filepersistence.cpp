/***********************************************************************************************************************
 * filepersistence.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"
#include "ModelBase/headers/test_nodes/BinaryNode.h"
#include "ModelBase/headers/test_nodes/BinaryNodePersistenceUnit.h"
#include "ModelBase/headers/test_nodes/PartialList.h"

#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( filepersistence, FilePersistence::FilePersistence )

inline void initFilePersistenceResources() { Q_INIT_RESOURCE(FilePersistence); }

namespace FilePersistence {

bool FilePersistence::initialize(Envision::EnvisionManager&)
{
	initFilePersistenceResources();
	return true;
}

void FilePersistence::selfTest(QString)
{
	TestNodes::BinaryNode::init();
	TestNodes::BinaryNodePersistenceUnit::init();
	TestNodes::PartialList::init();
	SelfTest::TestManager<FilePersistence>::runAllTests().printResultStatistics();
}

}
