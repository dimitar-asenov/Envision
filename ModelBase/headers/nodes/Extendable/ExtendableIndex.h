/***********************************************************************************************************************
 * ExtendableIndex.h
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDABLEINDEX_H_
#define EXTENDABLEINDEX_H_

#include "../../modelbase_api.h"

namespace Model {

class MODELBASE_API ExtendableIndex
{
	private:
		int level_;
		int index_;

	public:
		ExtendableIndex();
		ExtendableIndex(int level, int index);

		int level() const;
		int index() const;

		bool isValid() const;
};

}

#endif /* EXTENDABLEINDEX_H_ */
