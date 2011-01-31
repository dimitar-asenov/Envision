/***********************************************************************************************************************
 * Float.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FLOAT_H_
#define FLOAT_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class MODELBASE_API Float: public Node
{
	NODE_DECLARE_STANDARD_METHODS(Float)

	private:
		double value;

	public:

		double  get() const;
		void set(const double& newval);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
};

inline double Float::get() const { return value; }


}

#endif /* FLOAT_H_ */
