/***********************************************************************************************************************
 * PositionExtension.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POSITIONEXTENSION_H_
#define POSITIONEXTENSION_H_

#include "../modelbase_api.h"
#include "../nodes/Extendable/ExtendableNode.h"
#include "../nodes/Integer.h"
#include <QtCore/QVector>

namespace TestNodes {

class MODELBASE_API PositionExtension
{
	private:
		Model::ExtendableNode* self_;
		Model::ExtendableIndex xIndex;
		Model::ExtendableIndex yIndex;

		static int extensionId_;

	public:
		PositionExtension(Model::ExtendableNode* self, const QVector<Model::ExtendableIndex>& extensionAttributes);

		int x();
		int y();
		void set(int x, int y);

		static void registerExtension();
		static int extensionId();
		template <class T> static void extendNode(QVector<Model::ExtendableIndex>& extensionAttributes);
};

inline int PositionExtension::extensionId() { return extensionId_; }

template <class T> void PositionExtension::extendNode(QVector<Model::ExtendableIndex>& extensionAttributes)
{
	extensionAttributes.append(T::registerNewAttribute("x", "Integer", false, false, true));
	extensionAttributes.append(T::registerNewAttribute("y", "Integer", false, false, true));
}

inline int PositionExtension::x() { return static_cast<Model::Integer*> (self_->get(xIndex))->get(); }
inline int PositionExtension::y() { return static_cast<Model::Integer*> (self_->get(yIndex))->get(); }

}

#endif /* POSITIONEXTENSION_H_ */
