/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/
#include "JavaTypeSystem.h"

#include "../types/ArrayType.h"
#include "../types/ClassType.h"
#include "../types/NullType.h"
#include "../types/PrimitiveType.h"
#include "../types/StringType.h"
#include "../types/SymbolProviderType.h"

#include "../declarations/Class.h"
#include "../declarations/Module.h"
#include "../declarations/Project.h"

#include "ModelBase/src/nodes/TypedList.h"

using namespace Model;

namespace OOModel {

inline bool JavaTypeSystem::isStandardJavaLangClass(const QString& className, Model::Node* target)
{
	if (auto cl = DCast<Class>(target))
		if (cl->name() == className)
			if (auto langList = DCast<TypedList<Class>>(target->parent()))
				if (auto lang = DCast<Module>(langList->parent()))
					if (lang->name() == "lang")
						if (auto javaList = DCast<TypedList<Module>>(lang->parent()))
							if (auto java = DCast<Module>(javaList->parent()))
								if (java->name() == "java")
									if (auto projList = DCast<TypedList<Module>>(java->parent()))
										if (auto proj = DCast<Project>(projList->parent()))
											if (proj->parent() == nullptr)
												return true;

	return false;
}

TypeSystem::TypeRelations JavaTypeSystem::relationFirstToSecond(const Type* first, const Type* second)
{
	// Equality, including qualifiers
	if (first->equals(second)) return TypeRelations{EQUALTYPES};

	// Primitive types
	if (auto p = dynamic_cast<const PrimitiveType*>(first)) return relationPrimitiveToOther(p, second);
	if (auto p = dynamic_cast<const PrimitiveType*>(second)) return invert( relationPrimitiveToOther(p, first) );

	// Null types
	if (auto a = dynamic_cast<const NullType*>(first)) return relationNullToOther(a, second);
	if (auto a = dynamic_cast<const NullType*>(second)) return invert( relationNullToOther(a, first) );

	// Array types
	if (auto a = dynamic_cast<const ArrayType*>(first)) return relationArrayToOther(a, second);
	if (auto a = dynamic_cast<const ArrayType*>(second)) return invert( relationArrayToOther(a, first) );

	// Class types
	if (auto c = dynamic_cast<const ClassType*>(first)) return relationClassToOther(c, second);
	if (auto c = dynamic_cast<const ClassType*>(second)) return invert( relationClassToOther(c, first) );

	// Symbol provider types, must necessarily come after Class Types above
	if (auto sp = dynamic_cast<const SymbolProviderType*>(first)) return relationSymbolProviderToOther(sp, second);
	if (auto sp = dynamic_cast<const SymbolProviderType*>(second)) return invert( relationSymbolProviderToOther(sp,
																																					first) );

	// String types
	if (auto s = dynamic_cast<const StringType*>(first)) return relationStringToOther(s, second);
	if (auto s = dynamic_cast<const StringType*>(second)) return invert( relationStringToOther(s, first) );

	return None;
}

int JavaTypeSystem::primitiveTypeToSubtypingOrder(const PrimitiveType* primitive)
{
	switch (primitive->type())
	{
		case PrimitiveType::DOUBLE: return 0;
		case PrimitiveType::FLOAT: return 1;
		case PrimitiveType::LONG: return 2;
		case PrimitiveType::UNSIGNED_LONG: return 2;
		case PrimitiveType::INT: return 3;
		case PrimitiveType::UNSIGNED_INT: return 3;
		case PrimitiveType::CHAR: return 4;
		default: Q_ASSERT(false); break;
	}

	Q_ASSERT(false);
	return -1;
}

TypeSystem::TypeRelations JavaTypeSystem::relationPrimitiveToOther(const PrimitiveType* primitive,
		const Type* other)
{
	// Have in mind that at this point we know that the two types are not identical.
	// However there might be differences in const or valative qualifiers.

	if (auto otherP = dynamic_cast<const PrimitiveType*>(other))
	{
		if (primitive->type() == otherP->type()) return TypeRelations{EQUALTYPES};

		if (primitive->type() == PrimitiveType::BOOLEAN || primitive->type() == PrimitiveType::VOID)
			return None;

		if (otherP->type() == PrimitiveType::BOOLEAN || otherP->type() == PrimitiveType::VOID)
			return None;

		// Numeric types are convertible to each other
		TypeRelations ret{IsConvertibleTo | IsConvertibleFrom};

		// Numeric types also have some subtype relationships
		auto pOrder = primitiveTypeToSubtypingOrder(primitive);
		auto otherPOrder = primitiveTypeToSubtypingOrder(otherP);
		if ( pOrder < otherPOrder ) ret |= IsSupertype;
		else if ( pOrder > otherPOrder ) ret |= IsSubtype;
		else ret |= IsSupertype | IsSubtype;

		return ret;
	}
	else if (auto otherClass = dynamic_cast<const ClassType*>(other))
	{
		TypeRelations convertible{IsConvertibleTo | IsConvertibleFrom};

		if (primitive->type() == PrimitiveType::INT
						&& isStandardJavaLangClass("Integer", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::INT
						&& isStandardJavaLangClass("Byte", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::INT
						&& isStandardJavaLangClass("Short", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::LONG
						&& isStandardJavaLangClass("Long", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::UNSIGNED_INT
						&& isStandardJavaLangClass("Integer", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::UNSIGNED_LONG
						&& isStandardJavaLangClass("Long", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::FLOAT
						&& isStandardJavaLangClass("Float", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::DOUBLE
						&& isStandardJavaLangClass("Double", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::BOOLEAN
						&& isStandardJavaLangClass("Boolean", otherClass->classDefinition()))
					return convertible;
		if (primitive->type() == PrimitiveType::CHAR
						&& isStandardJavaLangClass("Character", otherClass->classDefinition()))
					return convertible;

		return None;
	}
	else return None;

}

TypeSystem::TypeRelations JavaTypeSystem::relationNullToOther(const NullType*, const Type* other)
{
	if (dynamic_cast<const NullType*>(other)) return TypeRelations{EQUALTYPES};
	if (dynamic_cast<const ClassType*>(other)) return IsConvertibleTo;
	if (dynamic_cast<const ArrayType*>(other)) return IsConvertibleTo;
	if (dynamic_cast<const StringType*>(other)) return IsConvertibleTo;

	return None;
}

TypeSystem::TypeRelations JavaTypeSystem::relationArrayToOther(const ArrayType* array, const Type* other)
{
	if (auto otherArray = dynamic_cast<const ArrayType*>(other))
	{
		auto elementRelation = relationFirstToSecond(array->elementType(), otherArray->elementType());

		if (elementRelation.testFlag(Equal)) return TypeRelations{EQUALTYPES};
		if (elementRelation.testFlag(IsSupertype)) return IsSupertype | IsConvertibleFrom;
		if (elementRelation.testFlag(IsSubtype)) return IsSubtype | IsConvertibleTo;

		return None;
	}

	if (auto otherClass = dynamic_cast<const ClassType*>(other))
	{
		if (isStandardJavaLangClass("Object", otherClass->classDefinition())) return IsSubtype | IsConvertibleTo;

		return None;
	}

	return None;
}

TypeSystem::TypeRelations JavaTypeSystem::relationStringToOther(const StringType*, const Type* other)
{
	if (dynamic_cast<const StringType*>(other)) return TypeRelations{EQUALTYPES};
	if (auto otherClass = dynamic_cast<const ClassType*>(other))
	{
		if (isStandardJavaLangClass("String", otherClass->classDefinition())) return IsConvertibleTo;
	}

	return None;
}

TypeSystem::TypeRelations JavaTypeSystem::relationSymbolProviderToOther(const SymbolProviderType* symbolProvider,
																								const Type*)
{
	Q_ASSERT(!DCast<Class>(symbolProvider->symbolProvider()));

	// These are things like modules, projects and method. None of these participate in the type system.
	return None;
}

TypeSystem::TypeRelations JavaTypeSystem::relationClassToOther(const ClassType* classType, const Type* other)
{
	if (auto otherClass = dynamic_cast<const ClassType*>(other))
	{
		if (classType->classDefinition() == otherClass->classDefinition()) return TypeRelations{EQUALTYPES};

		TypeRelations ret{None};
		for (auto thisBase : classType->classDefinition()->allBaseClasses(classType->typeArgumentBindings()))
			if (thisBase == otherClass->classDefinition())
			{
				ret |= IsSubtype | IsConvertibleTo;
				break;
			}

		for (auto otherBase : otherClass->classDefinition()->allBaseClasses(otherClass->typeArgumentBindings()))
			if (otherBase == classType->classDefinition())
			{
				ret |= IsSupertype | IsConvertibleFrom;
				break;
			}

		return ret;
	}

	return None;
}

}
