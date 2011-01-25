/***********************************************************************************************************************
 * AttributeChain.h
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ATTRIBUTECHAIN_H_
#define ATTRIBUTECHAIN_H_

#include "../../modelbase_api.h"
#include "Attribute.h"
#include "ExtendableIndex.h"

#include <QtCore/QString>
#include <QtCore/QVector>

namespace Model {

class MODELBASE_API AttributeChain: public QVector<Attribute>
{
	private:
		AttributeChain* parent_;

		int numLevels_; // The number of levels of attributes, including this one.

	public:
		AttributeChain();

		void setParent(AttributeChain* newParent);
		AttributeChain* parent();

		int recursiveSize() const;
		int numLevels() const;

		AttributeChain* level(int level);
		ExtendableIndex indexForAttribute(const QString &name) const;
		const Attribute& attribute(const ExtendableIndex &index);
		bool hasAttribute(const QString &name) const;
};

}

#endif /* ATTRIBUTECHAIN_H_ */
