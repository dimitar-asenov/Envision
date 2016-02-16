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

#include "filepersistence_api.h"
#include "XMLModel.h"

#include "ModelBase/src/persistence/ClipboardStore.h"

namespace Model {
	class Reference;
}

namespace FilePersistence {

class FILEPERSISTENCE_API SystemClipboard : public Model::ClipboardStore
{
	public:
		SystemClipboard();
		virtual ~SystemClipboard();

		/**
		 * Methods from Persistent Store
		 */
		virtual SystemClipboard* clone() const override;

		virtual void saveStringValue(const QString &value) override;
		virtual void saveIntValue(int value) override;
		virtual void saveDoubleValue(double value) override;
		virtual void saveReferenceValue(const QString &name, const Model::Node* target) override;
		virtual void saveNode(const Model::Node *node, const QString &name) override;

		virtual QList<Model::LoadedNode> loadAllSubNodes(Model::Node* parent, const QSet<QString>& loadPartially)override;
		virtual Model::Node* loadSubNode(Model::Node* parent, const QString& name, bool loadPartially) override;
		virtual QString currentNodeType() const override;

		virtual int loadIntValue() override;
		virtual QString loadStringValue() override;
		virtual double loadDoubleValue() override;
		virtual QString loadReferenceValue(Model::Reference* r) override;

		virtual bool isLoadingPartially() const override;

		/**
		 * Methods from ClipboardStore
		 */
		virtual void putNode(const Model::Node* node) override;
		virtual void putNodes(const QList<const Model::Node*>& nodes) override;

		virtual bool readClipboard() override;
		virtual int numNodes() const override;
		virtual bool hasNext() const override;
		virtual void next() override;
		virtual Model::Node* create(Model::TreeManager* manager, Model::Node* parent) override;

	protected:
		virtual void saveTree(Model::TreeManager* manager, const QString &name) override;
		virtual Model::Node* loadTree(Model::TreeManager* manager, const QString &name, bool loadPartially) override;

	private:
		XMLModel* xml;
		int numNodes_;

		Model::LoadedNode loadNode(Model::Node* parent);

		static const QString NULL_STRING;
};

}
