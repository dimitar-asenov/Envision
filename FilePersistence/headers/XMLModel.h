/***********************************************************************************************************************
 * XMLModel.h
 *
 *  Created on: Jan 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef XMLMODEL_H_
#define XMLMODEL_H_

#include "filepersistence_api.h"

#include "ModelBase/headers/persistence/PersistentStore.h"

#include <QtXml/QDomDocument>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QMutex>

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
		void setPartialHint(bool partialHint);

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
		bool getPartialHint() const;

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
};

}

#endif /* XMLMODEL_H_ */
