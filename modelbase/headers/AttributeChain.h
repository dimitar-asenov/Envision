/***********************************************************************************************************************
 * AttributeChain.h
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ATTRIBUTECHAIN_H_
#define ATTRIBUTECHAIN_H_

#include "modelbase_api.h"
#include "Attribute.h"
#include "ExtendableIndex.h"
#include <QVector>
#include <QString>

namespace Model {

class MODELBASE_API AttributeChain: public QVector<Attribute>
{
	private:
		AttributeChain* parent;

		int numLevels; // The number of levels of attributes, including this one.

	public:
		AttributeChain();

		void setParent(AttributeChain* newParent);
		AttributeChain* getParent();

		int getRecursiveSize() const;
		int getNumLevels() const;

		AttributeChain* getLevel(int level);
		ExtendableIndex getIndexForAttribute(const QString &name) const;
		const Attribute& getAttribute(const ExtendableIndex &index);
		bool hasAttribute(const QString &name) const;
};

}

#endif /* ATTRIBUTECHAIN_H_ */
