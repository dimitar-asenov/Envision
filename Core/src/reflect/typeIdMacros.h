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

#pragma once

#include "TypeRegistry.h"

/***********************************************************************************************************************
 * Declares standard methods for querying a classes's type statically or at run-time.
 *
 * DO NOT USE DIRECTLY. Use DECLARE_TYPE_ID or DECLARE_TYPE_ID_BASE defined below
 *
 * This macro declares a static method initType which should be called during the initialization of the a plug-in where
 * the class is defined. This will assure that the new class's type is properly initialized.
 *
 * This macro should appear as the first line after the class declaration e.g. :
 *
 * class MyNewNode : public ...
 * {
 * 	DECLARE_TYPE_ID
 * 	...
 *
 * hierarchyTypeIds:
 *		Returns an ordered list of all ids in the type hierarchy of this class.
 *		The most derived id appears at the front of the list.
 */
#define DECLARE_TYPE_ID_COMMON(OVERRIDE)																										\
	public:																																				\
		virtual const QString& typeName() const OVERRIDE;																					\
		virtual int typeId() const OVERRIDE;																									\
																																							\
		virtual QList<int> hierarchyTypeIds() const OVERRIDE;																				\
		virtual bool isSubtypeOf(int type) const OVERRIDE;																					\
		virtual bool isSubtypeOf(const QString& type) const OVERRIDE;																	\
																																							\
		static const QString& typeNameStatic();																								\
		static int typeIdStatic() { return typeId_; }																						\
		static void initType();																														\
																																							\
	private:																																				\
		static int typeId_;																															\

/***********************************************************************************************************************
 * A specialized version of DECLARE_TYPE_ID_COMMON that should be used with classes which inherit from a TYPE_ID enabled
 * class.
 *
 * This macro should appear as the first line after the class declaration e.g. :
 *
 * class MyNewNode : public ...
 * {
 * 	DECLARE_TYPE_ID
 * 	...
 */
#define DECLARE_TYPE_ID DECLARE_TYPE_ID_COMMON(override)

/***********************************************************************************************************************
 * A specialized version of DECLARE_TYPE_ID_COMMON that should be used with classes that are at the top of the
 * hierarchy.
 *
 * This macro should appear as the first line after the class declaration e.g. :
 *
 * class MyNewNode : public ...
 * {
 * 	DECLARE_TYPE_ID_BASE
 * 	...
 */
#define DECLARE_TYPE_ID_BASE DECLARE_TYPE_ID_COMMON()

/***********************************************************************************************************************
 * Defines standard methods for querying a classes's type statically or at run-time.
 *
 * DO NOT USE DIRECTLY. Use DEFINE_TYPE_ID_DERIVED or DEFINE_TYPE_ID_BASE defined below
 *
 * @param className
 * 			The name of the class being defined.
 *
 * @param globalPluginInitializationRegistryGetterFunction
 * 			The name of the function that should be used to retrieve the InitializationRegistry that will initialize
 * 			this type. This function must be defined in the same namespace as the defined class and should be linked
 * 			together with it in a single shared object.
 *
 * @param nameExpression
 * 			An expression that will be used to compute the registered string name of the class
 *
 * @param templatePrefix
 * 			If className designates a template class, then templatePrefix should be the usual template related code
 * 			that comes before member definitions.
 *
 *
 * Note that this macro does not define the initType method. This should be done accordingly for each type that is being
 * used. The initType() method should set the typeIdVariable() to the type's ID.
 *
 * This macro should appear in the .cpp file defining the className class.
 *
 * namespace MyNamespace {
 * DEFINE_TYPE_ID_COMMON
 * 	...
 */
#define DEFINE_TYPE_ID_COMMON(className, nameExpression, templatePrefix)															\
templatePrefix int className::typeId_ = ::Core::TypeRegistry::add<className>();													\
																																							\
templatePrefix const QString& className::typeName() const																				\
{																																							\
	return typeNameStatic();																														\
}																																							\
																																							\
templatePrefix int className::typeId()	const																									\
{																																							\
	return typeIdStatic();																															\
}																																							\
																																							\
templatePrefix const QString& className::typeNameStatic()																				\
{																																							\
	static QString typeName_{nameExpression};																									\
	return typeName_;																																	\
}																																							\

/***********************************************************************************************************************
 * A specialized version of DEFINE_TYPE_ID_COMMON that should be used with classes that are at the top of the hierarchy.
 *
 * This macro is typically used within other macros but can also appear directly in the .cpp file defining className.
 *
 * namespace MyNamespace {
 * DEFINE_TYPE_ID_COMMON
 * 	...
 */
#define DEFINE_TYPE_ID_BASE(className, nameExpression, templatePrefix)																\
DEFINE_TYPE_ID_COMMON(className, nameExpression, templatePrefix)																		\
																																							\
templatePrefix void className::initType() {}																									\
templatePrefix QList<int> className::hierarchyTypeIds() const																			\
{																																							\
	static QList<int> h = {typeIdStatic()};																									\
	return h;																																			\
}																																							\
templatePrefix bool className::isSubtypeOf(int type) const																				\
{																																							\
	return typeIdStatic() == type;																												\
}																																							\
templatePrefix bool className::isSubtypeOf(const QString& type) const																\
{																																							\
	return typeNameStatic() == type;																												\
}																																							\

/***********************************************************************************************************************
 * A specialized version of DEFINE_TYPE_ID_COMMON that should be used with classes which inherit from a TYPE_ID enabled
 * class.
 *
 * This macro is typically used within other macros but can also appear directly in the .cpp file defining className.
 *
 * namespace MyNamespace {
 * DEFINE_TYPE_ID_COMMON
 * 	...
 */
#define DEFINE_TYPE_ID_DERIVED(className, nameExpression, templatePrefix)															\
DEFINE_TYPE_ID_COMMON(className, nameExpression, templatePrefix)																		\
																																							\
templatePrefix QList<int> className::hierarchyTypeIds() const																			\
{																																							\
	static QList<int> h = QList<int>{} << typeIdStatic() << Super::hierarchyTypeIds();											\
	return h;																																			\
}																																							\
templatePrefix bool className::isSubtypeOf(int type) const																				\
{																																							\
	return typeIdStatic() == type || Super::hierarchyTypeIds().contains(type);														\
}																																							\
templatePrefix bool className::isSubtypeOf(const QString& type) const																\
{																																							\
	return typeNameStatic() == type || Super::isSubtypeOf(type);																		\
}
