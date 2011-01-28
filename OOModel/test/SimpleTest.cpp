/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oomodel.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "Class.h"

#include "ModelBase/headers/Model.h"

namespace OOModel {

TEST(OOModel, SimpleTest)
{
	Model::Model model;
	Class* root = dynamic_cast<Class*> (model.createRoot("Class"));

	CHECK_CONDITION(root != NULL);
	CHECK_CONDITION(root->name().isEmpty());
	model.beginModification(root, "setName");
	root->setName("Test");
	model.endModification();
	CHECK_STR_EQUAL("Test", root->name());
}

}
