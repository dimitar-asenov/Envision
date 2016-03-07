/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "AllTreeManagers.h"

#include "TreeManager.h"
#include "../nodes/Node.h"

namespace Model {

void AllTreeManagers::init()
{
	qRegisterMetaType< QSet<Node*> >("QSet<Node*>");
}

void AllTreeManagers::cleanup()
{
	// We make a copy of loadedManagers_ since any TreeManager will call remove() when being destroyed.
	auto copy = instance().loadedManagers_;
	instance().loadedManagers_.clear();

	for (auto m : copy) SAFE_DELETE(m);
}

AllTreeManagers& AllTreeManagers::instance()
{
	static AllTreeManagers manager;
	return manager;
}

AllTreeManagers::AllTreeManagers()
{}

AllTreeManagers::~AllTreeManagers()
{
	Q_ASSERT(loadedManagers_.isEmpty());
}

TreeManager* AllTreeManagers::find(Node* root) const
{
	// TODO: use auto in places like this which requires that we can resolve libraries needed for export to detect
	// a dependency
	for (TreeManager* m : loadedManagers_)
		if ( m->root() == root ) return m;

	return nullptr;
}

TreeManager* AllTreeManagers::managerOfOwningUndoStack(Node* node,  const NodeOwningCommand* excludeCommand) const
{
	for (auto m : loadedManagers_)
		if (m->isOwnedByUndoStack(node, excludeCommand)) return m;
	return nullptr;
}

void AllTreeManagers::add(TreeManager* manager)
{
	if (!loadedManagers_.contains(manager)) loadedManagers_.append(manager);
}

TreeManager* AllTreeManagers::remove(TreeManager* manager)
{
	loadedManagers_.removeOne(manager);
	return manager;
}

Node* AllTreeManagers::nodeForId(NodeIdType id) const
{
	for (auto manager : loadedManagers())
		if (auto node = manager->nodeIdMap().node(id))
			return const_cast<Node*>(node);
	return nullptr;
}

}
