/***********************************************************************************************************************
 * Character.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CHARACTER_H_
#define CHARACTER_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class MODELBASE_API Character: public Node
{
	NODE_DECLARE_STANDARD_METHODS(Character)

	private:
		QChar value;

	public:

		QChar get() const;
		void set(const QChar& newval);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
};

inline QChar Character::get() const { return value; }
}

#endif /* CHARACTER_H_ */
