/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../src/ModelBasePlugin.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "../src/test_nodes/BinaryNode.h"
#include "../src/model/TreeManager.h"
#include "../src/nodes/Integer.h"
#include "../src/nodes/NameText.h"
#include "PersistentStoreMock.h"

namespace Model {

class MODELBASE_API PersistenceSave : public Test<ModelBasePlugin, PersistenceSave> { public: void test()
{
	auto root = new TestNodes::BinaryNode{};
	TreeManager manager{"root", root};
	PersistentStoreMock store;
	manager.save(&store);

	CHECK_STR_EQUAL("BinaryNode,root,TypedListOfText,_ext_TagExtension_tags,NameText,name,"
						 ",Integer,_ext_PositionExtension_x,0,Integer,_ext_PositionExtension_y,0,",
			store.getSaved());

	manager.beginModification(root, "make tree");
	root->setLeft(new TestNodes::BinaryNode{});
	root->setRight( new TestNodes::BinaryNode{});
	root->name()->set("Troot");
	root->left()->name()->set("Tleft");
	root->right()->name()->set("Tright");
	manager.endModification();

	store.clear();
	manager.save();

	CHECK_STR_EQUAL("BinaryNode,root,TypedListOfText,_ext_TagExtension_tags,NameText,name,Troot,BinaryNode,left,"
						 "TypedListOfText,_ext_TagExtension_tags,NameText,name,Tleft,Integer,_ext_PositionExtension_x,0,"
						 "Integer,_ext_PositionExtension_y,0,BinaryNode,right,TypedListOfText,_ext_TagExtension_tags,"
						 "NameText,name,Tright,Integer,_ext_PositionExtension_x,0,Integer,_ext_PositionExtension_y,0,"
						 "Integer,_ext_PositionExtension_x,0,Integer,_ext_PositionExtension_y,0,",
			store.getSaved());
}};

}
