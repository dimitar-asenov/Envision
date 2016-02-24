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

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class FILEPERSISTENCE_API XMLModel
{
	public:
		XMLModel();
		XMLModel(const QString& filename);
		virtual ~XMLModel();

		void saveStringValue(const QString &value);
		void saveIntValue(int value);
		void saveDoubleValue(double value);

		void beginSaveChildNode(const QString &tag);
		void endSaveChildNode();
		void importChildFromXML(QDomElement child);
		void saveNext(const QString& tag);

		void setId(Model::NodeIdType id);
		void setName(const QString& name);

		int loadIntValue() const;
		QString loadStringValue() const;
		double loadDoubleValue() const;

		bool hasChild(const QString& nodeName) const;
		bool hasChildren() const;
		void beginLoadChildNode(const QString& nodeName);
		void endLoadChildNode();
		QStringList getChildrenNames() const;

		bool hasNext() const;
		void loadNext();
		bool goToElement(Model::NodeIdType id, bool startFromRoot = true);
		void goToFirstChild();
		void goToParent();
		void goToRoot();

		QString getType() const;
		Model::NodeIdType getId() const;
		QString getName() const;

		QDomElement getCurrentElement() const;

		bool isString() const;
		bool isInteger() const;
		bool isDouble() const;

		QString documentText() const;
		void setDocumentText(const QString& text);

	private:
		QDomDocument doc;
		QDomElement elem;
		QList<QDomElement> elemStack;

		static const char* XML_DOM_TYPE;
		static const int MAX_DOUBLE_PRECISION;
};

}
