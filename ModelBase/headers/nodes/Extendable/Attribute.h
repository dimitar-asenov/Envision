/***********************************************************************************************************************
 * Attribute.h
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include "../../modelbase_api.h"
#include <QtCore/QString>

namespace Model {

class MODELBASE_API Attribute
{
	private:
		QString name_;
		QString type_;
		bool optional_;
		bool partialHint_;
		bool persistent_;

	public:
		Attribute();

		Attribute(QString name, QString type, bool optional, bool partialHint, bool persistent);

		/**
		 * Used for comparisons only.
		 */
		Attribute(QString name);

		const QString& name() const;
		const QString& type() const;
		bool optional() const;
		bool partialHint() const;
		bool persistent() const;

		bool operator==(const Attribute &other) const;
};

}

#endif /* ATTRIBUTE_H_ */
