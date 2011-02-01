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
	NODE_DECLARE_STANDARD_METHODS( Reference )

	private:
		QString path_;

	public:

		Node* get();
		void set(Node* target);
		void set(const QString &path);

		const QString& path() const;

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);

	protected:
		virtual Node* getTargetFromSymbolicPath(const QString &path);
		virtual QString getSymbolicPathToTarget(Node* target);

		// These are the default implementation of the symbolic lookup functions
		Node* getTargetFromLocalPath(const QString &path);
		QString getLocalPathToTarget(Node* target);
};

inline Node* Reference::get() { return getTargetFromSymbolicPath(path_); }
inline const QString& Reference::path() const { return path_; }

}

#endif /* REFERENCE_H_ */
