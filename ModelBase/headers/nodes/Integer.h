/***********************************************************************************************************************
 * Integer.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INTEGER_H_
#define INTEGER_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class MODELBASE_API Integer: public Node
{
	NODE_DECLARE_STANDARD_CONSTRUCTORS(Integer)

	private:
		int integer;

	public:

		int  get() const;
		void set(int newval);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
};

}

#endif /* INTEGER_H_ */
