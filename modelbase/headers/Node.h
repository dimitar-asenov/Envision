/***********************************************************************************************************************
 * Node.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NODE_H_
#define NODE_H_

#include "modelbase_api.h"
#include "Persistent.h"

namespace Model {

class MODELBASE_API Node : public Persistent
{
	private:
		Node* parent;

	public:
		Node(Node* parent);
		virtual ~Node();
};

}

#endif /* NODE_H_ */
