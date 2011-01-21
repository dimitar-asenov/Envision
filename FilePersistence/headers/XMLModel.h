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
		virtual ~XMLModel();

		void saveStringValue(const QString &value);
		void saveIntValue(int value);
		void saveFloatValue(double value);

		void beginSaveNode(const QString &tag);
		void endSaveNode();

		void setId(Model::NodeIdType id);
		void setName(const QString& name);
		void setPartialHint(bool partialHint);

		int loadIntValue();
		QString loadStringValue();
		double loadFloatValue();

		bool hasChildNode(const QString& nodeName);
		void beginLoadChildNode(const QString& nodeName);
		void endLoadChildNode();

		bool hasNext();
		void next();

		QString getType();
		Model::NodeIdType getId();
		QString getName();
		bool getPartialHint();

		QString documentText();
		void setDocument(const QString& text);
		void setDocument(QFile file);

	private:
		QDomDocument doc;
		QDomElement elem;
		QList<QDomElement> elemStack;
};

}

#endif /* XMLMODEL_H_ */
