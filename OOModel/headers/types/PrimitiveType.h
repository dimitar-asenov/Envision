/***********************************************************************************************************************
 * PrimitiveType.h
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PRIMITIVETYPE_H_
#define PRIMITIVETYPE_H_

#include "Type.h"

#include "ModelBase/headers/nodes/Integer.h"

namespace OOModel {

class OOMODEL_API PrimitiveType : public Type
{
	EXTENDABLENODE_DECLARE_STANDARD_METHODS(PrimitiveType)
	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, val, setVal, int);

	public:
		enum PrimitiveTypes {INT, LONG, UNSIGNED_INT, UNSIGNED_LONG, FLOAT, DOUBLE, BOOLEAN, CHAR, VOID};

		PrimitiveTypes type() const;
		void setType(const PrimitiveTypes& type);
};

inline PrimitiveType::PrimitiveTypes PrimitiveType::type() const { return static_cast<PrimitiveTypes> (val()); }
inline void PrimitiveType::setType(const PrimitiveTypes& type) { setVal(type); }

}

#endif /* PRIMITIVETYPE_H_ */
