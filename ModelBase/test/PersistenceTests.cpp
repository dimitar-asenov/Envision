/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "modelbase.h"
#include "SelfTest/src/SelfTestSuite.h"
#include "test_nodes/BinaryNode.h"
#include "model/Model.h"
#include "nodes/Integer.h"
#include "nodes/Text.h"
#include "PersistentStoreMock.h"

namespace Model {

TEST(ModelBase, PersistenceSave)
{
	auto root = new TestNodes::BinaryNode();
	Model model("root", root);
	PersistentStoreMock store;
	model.save(&store);

	CHECK_STR_EQUAL("BinaryNode,root,full,Text,name,full,,Integer,_ext_PositionExtension_x,full,0,Integer,_ext_PositionExtension_y,full,0,", store.getSaved());

	model.beginModification(root, "make tree");
	root->setLeft(new TestNodes::BinaryNode());
	root->setRight( new TestNodes::BinaryNode());
	root->name()->set("Troot");
	root->left()->name()->set("Tleft");
	root->right()->name()->set("Tright");
	model.endModification();

	store.clear();
	model.save();

	CHECK_STR_EQUAL("BinaryNode,root,full,Text,name,full,Troot,BinaryNode,left,full,Text,name,full,Tleft,Integer,_ext_PositionExtension_x,full,0,Integer,_ext_PositionExtension_y,full,0,BinaryNode,right,full,Text,name,full,Tright,Integer,_ext_PositionExtension_x,full,0,Integer,_ext_PositionExtension_y,full,0,Integer,_ext_PositionExtension_x,full,0,Integer,_ext_PositionExtension_y,full,0,", store.getSaved());
}

}
