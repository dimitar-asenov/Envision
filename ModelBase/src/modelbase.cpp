/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * modelbase.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "Model.h"

#include "nodes/Text.h"
#include "nodes/Integer.h"
#include "nodes/Float.h"
#include "nodes/Boolean.h"
#include "nodes/Character.h"
#include "nodes/Reference.h"
#include "nodes/Extendable/ExtendableNode.h"
#include "nodes/List.h"
#include "nodes/TypedList.h"

#include "test_nodes/BinaryNode.h"
#include "test_nodes/BinaryNodeAccessUnit.h"
#include "test_nodes/PositionExtension.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( modelbase, Model::ModelBase )

namespace Model {

Log* ModelBase::logger = nullptr;

bool ModelBase::initialize(Envision::EnvisionManager&)
{
	Model::init();

	logger = Logger::Log::getLogger("modelbase");
	Text::registerNodeType();
	Integer::registerNodeType();
	Float::registerNodeType();
	Boolean::registerNodeType();
	Character::registerNodeType();
	Reference::registerNodeType();
	ExtendableNode::registerNodeType();
	List::registerNodeType();

	TypedList<Text>::registerNodeType();
	TypedList<Integer>::registerNodeType();
	TypedList<Float>::registerNodeType();
	TypedList<Boolean>::registerNodeType();
	TypedList<Character>::registerNodeType();
	TypedList<Reference>::registerNodeType();

	return true;
}

void ModelBase::selfTest(QString)
{
	TestNodes::BinaryNode::init();
	TestNodes::BinaryNodeAccessUnit::init();

	TestNodes::PositionExtension::registerExtension();
	TestNodes::BinaryNode::registerNewExtension<TestNodes::PositionExtension>();

	SelfTest::TestManager<ModelBase>::runAllTests().printResultStatistics();
}

Log* ModelBase::log()
{
	return logger;
}

}
