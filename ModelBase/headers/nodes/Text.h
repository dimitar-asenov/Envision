/***********************************************************************************************************************
 * Text.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODEL_TEXT_H_
#define MODEL_TEXT_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class NameChange;
template<class T> class FieldSet;

class MODELBASE_API Text: public Node
{
	friend class NameChange;

	NODE_DECLARE_STANDARD_CONSTRUCTORS(Text)

	private:
		QString text;

		FieldSet<QString>* getSetCommand(const QString& newText);
	public:

		const  QString&  get() const;
		operator const QString&() const;

		void set(const QString &newText);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
};

}

#endif /* MODEL_TEXT_H_ */
