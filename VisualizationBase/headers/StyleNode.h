/***********************************************************************************************************************
 * StyleNode.h
 *
 *  Created on: Dec 20, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STYLENODE_H_
#define STYLENODE_H_

#include "visualizationbase_api.h"

#include <QtXml/QDomDocument>
#include <QtCore/QList>
#include <QtCore/QStringList>
namespace Visualization {

class VISUALIZATIONBASE_API StyleNode
{
	private:
		static QString baseFolder;

		StyleNode* parent;

		QDomDocument doc;
		QString folder;

		QDomElement elem;
		QList< StyleNode* > prototypes;
		QList< StyleNode* > children;

		StyleNode(StyleNode* parent, QDomElement elem);
		StyleNode(StyleNode* parent, const QString& prototypeName, const QString& currentFolder);
		void init();

		QDomDocument openStyleDoc(const QString& path);
		QString getFolder();
		QDomElement getElement(QStringList path);

	public:
		StyleNode(const QString& rootStyleFileName, const QString& folderWithinStyles);
		virtual ~StyleNode();

		static void setBaseFolder( const QString &baseFolder);

		QString getProperty(QStringList path);
};

}

#endif /* STYLENODE_H_ */
