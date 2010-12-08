/***********************************************************************************************************************
 * Text.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXT_H_
#define TEXT_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class NameChange;

class MODELBASE_API Text: public Node
{
	friend class NameChange;

	NODE_DECLARE_STANDARD_CONSTRUCTORS(Text)

	private:
		QString text;

	public:

		const  QString&  get() const;
		operator const QString&() const;

		void set(const QString &newText);

		void save(PersistentStore &store) const;
};

}

#endif /* TEXT_H_ */
