/***********************************************************************************************************************
 * Boolean.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOOLEAN_H_
#define BOOLEAN_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class MODELBASE_API Boolean: public Node
{
	NODE_DECLARE_STANDARD_METHODS(Boolean)

	private:
		bool value;

	public:

		bool get() const;
		void set(const bool& newval);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
};

inline bool Boolean::get() const { return value; }

}

#endif /* BOOLEAN_H_ */
