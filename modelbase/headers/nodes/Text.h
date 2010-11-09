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

class Text: public Model::Node
{
	NODE_DECLARE_STANDARD_CONSTRUCTORS(Text)

	private:
		QString text;

	public:
		QString get();
		void set(const QString &newText);

		void save(PersistentStore &store);
};

}

#endif /* TEXT_H_ */
