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

#include "../filepersistence_api.h"

namespace FilePersistence {

class GenericTree;
class GenericNode;

class FILEPERSISTENCE_API GenericPersistentUnit {
	public:
		~GenericPersistentUnit();

		GenericPersistentUnit(const GenericPersistentUnit& other) = delete;

		const QString& name() const;
		GenericTree* tree() const;

		GenericNode* newNode();
		GenericNode* newNode(int lineStart, int lineEndEnclusive);
		GenericNode* newNode(const char* data, int dataLength);
		/**
		 * If \a force is true then no check for ID uniqueness is performed.
		 */
		GenericNode* newNode(const GenericNode* nodeToCopy, bool force = false, bool deepCopy = false);
		/**
		 * Returns the node according to \a data. If the boolean return value is true, the node was newly created
		 * and needs to be linked; Otherwise such a node already exists in the tree and that node is returned. In this
		 * case the node does not need to be linked.
		 */
		QPair<bool, GenericNode*> newOrExistingNode(const char* data, int dataLength);
		/**
		 * Copies the provided \a data to be used for initializing child GenericNode elements. The copy will be
		 * destroyed with the object.
		 *
		 * Returns a pointer to the copied data.
		 */
		const char* setData(const char* data, int dataSize);

		/**
		 * Returns the root node for this persistence unit under the assumption that all nodes in this unit have been
		 * loaded.
		 */
		GenericNode* unitRootNode() const;

		/**
		 * Returns the node that has no parent (parent is {000...}). This is typically the root node of the tree.
		 */
		GenericNode* nodeWithNullParent() const;

		QString extractRootNodeIDFromName() const;

	private:
		friend class GenericTree;
		friend class PiecewiseLoader;
		GenericPersistentUnit(GenericTree* tree, QString name, char* data = nullptr, int dataSize = 0);

		GenericTree* tree_{};
		QString name_;
		char* data_{};
		int dataSize_{};

		QList<GenericNode*> chunks_;
		int lastNodeIndexInLastChunk_{};

		GenericNode* nextNode();
		void releaseLastNode();
};

inline GenericTree* GenericPersistentUnit::tree() const { return tree_; }
inline const QString& GenericPersistentUnit::name() const { return name_; }

}
