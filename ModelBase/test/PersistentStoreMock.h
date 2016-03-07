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

#include "../src/modelbase_api.h"

#include "../src/persistence/PersistentStore.h"

namespace Model {

class Node;
class TreeManager;
class Reference;

class MODELBASE_API PersistentStoreMock: public PersistentStore
{
	private:
		QString savedData;

	public:
		PersistentStoreMock();
		~PersistentStoreMock();

		virtual PersistentStore* clone() const override;

		virtual void saveStringValue(const QString &value) override;
		virtual void saveIntValue(int value) override;
		virtual void saveDoubleValue(double value) override;
		virtual void saveNode(const Node *node, const QString &name) override;

		virtual QList<LoadedNode> loadAllSubNodes(Node* parent, const QSet<QString>& loadPartially) override;
		virtual Node* loadSubNode(Node* parent, const QString& name, bool loadPartially) override;
		virtual QString currentNodeType() const override;

		virtual int loadIntValue() override;
		virtual QString loadStringValue() override;
		virtual double loadDoubleValue() override;

		const QString& getSaved() const;
		void clear();

		virtual QString loadReferenceValue(Reference* r) override;
		virtual void saveReferenceValue(const QString &name, const Node* target) override;

		virtual bool isLoadingPartially() const override;

	protected:
		virtual void saveTree(TreeManager* manager, const QString &name) override;
		virtual Node* loadTree(TreeManager* manager, const QString &name, bool loadPartially) override;

};

}
