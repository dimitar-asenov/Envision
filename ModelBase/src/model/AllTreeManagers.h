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

#pragma once

#include "../modelbase_api.h"

#include "../persistence/PersistentStore.h"

namespace Model {

class TreeManager;
class Node;
class NodeOwningCommand;

/**
 * The AllTreeManagers class contains all existing tree managers.
 */
class MODELBASE_API AllTreeManagers {
	friend class TreeManager;

	public:
		~AllTreeManagers();
		AllTreeManagers(const AllTreeManagers&) = delete;
		AllTreeManagers& operator=(const AllTreeManagers&) = delete;

		/**
		 * Registers types with the meta object system of Qt to allow signals and slots to work with lists.
		 */
		static void init();

		/**
		 * Closes all loaded tree managers.
		 */
		static void cleanup();

		static AllTreeManagers& instance();

		/**
		 * Returns the TreeManager object that has as its root node the node indicated.
		 */
		TreeManager* find(Node* root) const;

		/**
		 * Returns a list to all currently loaded tree managers.
		 */
		const QList<TreeManager*>& loadedManagers() const;

		TreeManager* managerOfOwningUndoStack(Node* node, const NodeOwningCommand* excludeCommand = nullptr) const;

		Node* nodeForId(NodeIdType id) const;

		NodeIdType idForNode(Node* node) const;

	private:
		AllTreeManagers();

		/**
		 * Adds \a manager the list of tree managers. This manager will take ownership of \a manager.
		 *
		 * Newly created instances of TreeManager call this method in their constructors.
		 */
		void add(TreeManager* manager);

		/**
		 * Removes \a manager from the list of tree managers. This manager will give up ownership of \a manager.
		 *
		 * The removed tree manager is returned. This method is called in the desctructor of TreeManager.
		 */
		TreeManager* remove(TreeManager* manager);

		/**
		 * A list of all TreeManager objects that are currently instantiated. This is used to find the TreeManager
		 * corresponding to a particular root object.
		 */
		QList<TreeManager*> loadedManagers_;
};

inline const QList<TreeManager*>& AllTreeManagers::loadedManagers() const { return loadedManagers_; }

}
