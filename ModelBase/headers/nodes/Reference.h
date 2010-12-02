/***********************************************************************************************************************
 * Reference.h
 *
 *  Created on: Nov 17, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "Node.h"
#include "nodeMacros.h"

#include <QtCore/QString>

namespace Model {

class MODELBASE_API Reference: public Node
{
	NODE_DECLARE_STANDARD_CONSTRUCTORS( Reference )

	private:
		QString path;

	public:

		Node* get();
		void set(Node* target);
		void set(const QString &path);

		void save(PersistentStore &store) const;

	protected:
		virtual Node* getTargetFromSymbolicPath(const QString &path);
		virtual QString getSymbolicPathToTarget(Node* target);

		// These are the default implementation of the symbolic lookup functions
		Node* getTargetFromLocalPath(const QString &path);
		QString getLocalPathToTarget(Node* target);

};

}

#endif /* REFERENCE_H_ */
