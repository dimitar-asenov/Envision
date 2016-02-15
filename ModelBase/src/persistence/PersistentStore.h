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

#pragma once

#include "../modelbase_api.h"

namespace Model {

using NodeIdType = QUuid;

class Node;
class TreeManager;
class Reference;

struct MODELBASE_API LoadedNode
{
	QString name;
	Node* node;
};

class MODELBASE_API PersistentStore
{
	friend class TreeManager;

	public:
		virtual void saveStringValue(const QString &value) = 0;
		virtual void saveIntValue(int value) = 0;
		virtual void saveDoubleValue(double value) = 0;
		virtual void saveReferenceValue(const QString &name, const Node* target) = 0;
		virtual void saveNode(const Node *node, const QString &name) = 0;

		virtual QList<LoadedNode> loadAllSubNodes(Node* parent, const QSet<QString>& loadPartially) = 0;
		virtual Node* loadSubNode(Node* parent, const QString& name, bool loadPartially) = 0;
		virtual QString currentNodeType() const = 0;

		virtual int loadIntValue() = 0;
		virtual QString loadStringValue() = 0;
		virtual double loadDoubleValue() = 0;

		virtual bool isLoadingPartially() const = 0;

		/**
		 * \brief Returns a null string if the reference is resolved and a symbolic name if the reference is unresolved.
		 *
		 * After the target of the reference exists in memory, the reference will be updated to point to the correct
		 * target.
		 */
		virtual QString loadReferenceValue(Reference* r) = 0;

		virtual ~PersistentStore();
		virtual PersistentStore* clone() const = 0;

	protected:
		virtual void saveTree(TreeManager* manager, const QString &name) = 0;
		virtual Node* loadTree(TreeManager* manager, const QString &name, bool loadPartially) = 0;

		void setReferenceTargetr(Reference* r, Node* target);
};

}
