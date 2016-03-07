/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#pragma once

#include "../modelbase_api.h"

#include "Node.h"
#include "composite/Attribute.h"
#include "composite/AttributeChain.h"
#include "composite/CompositeIndex.h"
#include "composite/CompositeNode.h"
#include "../persistence/PersistentStore.h"

#include "Core/src/reflect/typeIdMacros.h"
#include "Core/src/reflect/Reflect.h"

/**
 * Declares standard constructors and standard static methods for registering the constructors of a class inheriting
 * Node.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from Node, directly or indirectly. For
 * 			classes directly inheriting CompositeNode use the macro COMPOSITENODE_DECLARE_STANDARD_CONSTRUCTORS.
 *
 * This macro declares a static method initType() which should be called during the initialization of
 * the plug-in where this node is defined. This will assure that the new node type's constructors are properly
 * registered.
 *
 * This macro should appear as the first line after the class declaration e.g. :
 *
 * class MyNewNode : public Node
 * {
 * 	NODE_DECLARE_STANDARD_METHODS( MyNewNode )
 */
#define NODE_DECLARE_STANDARD_METHODS(className)																							\
	DECLARE_TYPE_ID																																	\
	public:																																				\
		className(::Model::Node* parent = nullptr);																							\
		className(::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially);									\
		className* clone() const override;																										\
		static className* createDefaultInstance(Node* parent = nullptr);																\

/**
 * Declares standard constructors and standard static methods for registering the constructors of a class inheriting
 * CompositeNode.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from CompositeNode, directly or indirectly.
 *
 * This macro declares a static method initType() which should be called during the initialization of
 * the plug-in where this node is defined. This will assure that the new node type's constructors are properly
 * registered.
 *
 * This macro should appear as the first line after the class declaration e.g. :
 *
 * class MyNewNode : public CompositeNode
 * {
 * 	COMPOSITENODE_DECLARE_STANDARD_METHODS( MyNewNode )
 */
#define COMPOSITENODE_DECLARE_STANDARD_METHODS(className)																				\
	NODE_DECLARE_STANDARD_METHODS(className)																									\
	public:																																				\
		className(::Model::Node* parent, ::Model::AttributeChain& metaData);															\
		className(::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially,									\
			::Model::AttributeChain& metaData); 																								\
																																							\
		static ::Model::AttributeChain& getMetaData();																						\
		static ::Model::CompositeIndex registerNewAttribute(const QString &attributeName,										\
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent);				\
																																							\
		static ::Model::CompositeIndex registerNewAttribute(const ::Model::Attribute& attribute);								\
																																							\
		template <typename T> static void registerNewExtension()																				\
		{																																					\
			if (getMetaData().hasExtensionInHierarchy(T::extensionId()) == false) 													\
				T::template extendNode< className >(getMetaData().addExtension(T::extensionId()));								\
		}																																					\
																																							\
	protected:																																			\
		virtual ::Model::AttributeChain& topLevelMeta() override;																		\
																																							\
	private:																																				\
		static QList<QPair< ::Model::CompositeIndex&, ::Model::Attribute> >& attributesToRegisterAtInitialization_();	\
		static ::Model::CompositeIndex addAttributeToInitialRegistrationList_														\
				(::Model::CompositeIndex& index, const QString &attributeName, const QString &attributeType, 				\
					bool canBePartiallyLoaded, bool isOptional, bool isPersistent);													\

/**
 * Defines standard empty constructors for a new Node type which just call their parent constructors.
 *
 * @param className
 * 			The name of the class being defined. This class must be or inherit from from Node, directly or indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define DEFINE_NODE_EMPTY_CONSTRUCTORS(className)																							\
	className::className(::Model::Node* parent) : Super{parent} {}																		\
																																							\
	className::className(::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially)						\
		: Super {parent, store, loadPartially} {}																								\
																																							\
	className* className::clone() const { return new className{*this}; }

/**
 * Defines standard empty constructors for a new Node type which just call their parent constructors.
 *
 * @param className
 * 			The name of the class being defined. This class must be or inherit from from CompositeNode, directly or
 * 			indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(className)																				\
	className::className(::Model::Node* parent)																								\
		: Super (parent, className::getMetaData()) {}																						\
																																							\
	className::className(::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially)						\
		: Super (parent, store, loadPartially, className::getMetaData()) {}															\
																																							\
	className* className::clone() const { return new className{*this}; }																\
																																							\
	className::className(::Model::Node* parent, ::Model::AttributeChain& metaData)												\
		: Super {parent, metaData} {}																												\
																																							\
	className::className(::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially,						\
			::Model::AttributeChain& metaData)																									\
		: Super {parent, store, loadPartially, metaData} {}

/**
 * This is just the common subpart of the two macros below. Do not use it directly.
 */
#define DEFINE_NODE_TYPE_REGISTRATION_METHODS_COMMON(className)																		\
DEFINE_TYPE_ID_DERIVED(className, #className, )																								\
																																							\
void className::initType()																															\
{																																							\
	Node::registerNodeType(																															\
		#className,																																		\
		[](::Model::Node* parent) -> ::Model::Node* { return className::createDefaultInstance(parent); },					\
		[](::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially)-> ::Model::Node*						\
			{ return new className{parent, store, loadPartially};}																		\
	);																																						\
}																																							\

/**
 * Defines standard static methods that register the new Node type's constructors and a virtual getTypeName method
 * that returns the name of this class.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from from Node, directly or indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define DEFINE_NODE_TYPE_REGISTRATION_METHODS(className)																					\
DEFINE_NODE_TYPE_REGISTRATION_METHODS_COMMON(className)																					\
className* className::createDefaultInstance( Node* parent) { return new className{parent}; }									\


/**
 * Defines standard static methods that register the new Node type's constructors and a virtual getTypeName method
 * that returns the name of this class.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from from Node, directly or indirectly.
 *
 * @param proxyClassName
 * 			The name of a class that should be created when a default instance of the current class is requested.
 * 			Typically this is identical to className, but in special cases it can be different. E.g. when a default
 * 			Expression is requested, what actually will be constructed is an EmptryExpression.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define DEFINE_NODE_TYPE_REGISTRATION_METHODS_WITH_DEFAULT_PROXY(className, proxyClassName)									\
DEFINE_NODE_TYPE_REGISTRATION_METHODS_COMMON(className)																					\
className* className::createDefaultInstance( Node* parent) { return proxyClassName::createDefaultInstance(parent);}	\

/**
 * This is just the common subpart of the two macros below. Do not use it directly.
 */
#define DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS_COMMON(className)															\
DEFINE_TYPE_ID_DERIVED(className, #className, )																								\
																																							\
void className::initType()																															\
{																																							\
	Node::registerNodeType(																															\
		#className,																																		\
		[](::Model::Node* parent) -> ::Model::Node* { return className::createDefaultInstance(parent); },					\
		[](::Model::Node *parent, ::Model::PersistentStore &store, bool loadPartially)	-> ::Model::Node* 				\
			{ return new className{parent, store, loadPartially};}																		\
	);																																						\
																																							\
	for (int i = 0; i<attributesToRegisterAtInitialization_().size(); ++i)															\
		attributesToRegisterAtInitialization_().at(i).first =																				\
				registerNewAttribute(attributesToRegisterAtInitialization_().at(i).second);										\
}																																							\
																																							\
::Model::AttributeChain& className::getMetaData()																							\
{																																							\
	static ::Model::AttributeChain descriptions{#className, &Super::getMetaData()};												\
	return descriptions;																																\
}																																							\
																																							\
::Model::AttributeChain& className::topLevelMeta()																							\
{																																							\
	return getMetaData();																															\
}																																							\
																																							\
QList<QPair< ::Model::CompositeIndex&, ::Model::Attribute> >& className::attributesToRegisterAtInitialization_()		\
{																																							\
	static QList<QPair< ::Model::CompositeIndex&, ::Model::Attribute> > a;															\
	return a;																																			\
}																																							\
																																							\
::Model::CompositeIndex className::registerNewAttribute(const QString &attributeName,											\
	const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)								\
{																																							\
	return CompositeNode::registerNewAttribute(getMetaData(), attributeName, attributeType, 									\
					canBePartiallyLoaded, isOptional, isPersistent );																		\
}																																							\
																																							\
::Model::CompositeIndex className::registerNewAttribute(const ::Model::Attribute& attribute)									\
{																																							\
	return CompositeNode::registerNewAttribute(getMetaData(), attribute);															\
}																																							\
																																							\
::Model::CompositeIndex className::addAttributeToInitialRegistrationList_ (::Model::CompositeIndex& index,				\
	const QString &attributeName, const QString &attributeType, bool canBePartiallyLoaded, bool isOptional,				\
			 bool isPersistent)																														\
{																																							\
	attributesToRegisterAtInitialization_().append(QPair< ::Model::CompositeIndex&, ::Model::Attribute>{index,			\
			::Model::Attribute{attributeName, attributeType, isOptional, canBePartiallyLoaded, isPersistent}});			\
	return ::Model::CompositeIndex{};																											\
}																																							\

/**
 * Defines standard static methods that register the new Node type's constructors and a virtual getTypeName method
 * that returns the name of this class.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from CompositeNode, directly or indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(className)																		\
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS_COMMON(className)																		\
className* className::createDefaultInstance( Node* parent) { return new className{parent}; }									\

/**
 * Defines standard static methods that register the new Node type's constructors and a virtual getTypeName method
 * that returns the name of this class.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from CompositeNode, directly or indirectly.
 *
 * @param proxyClassName
 * 			The name of a class that should be created when a default instance of the current class is requested.
 * 			Typically this is identical to className, but in special cases it can be different. E.g. when a default
 * 			Expression is requested, what actually will be constructed is an EmptryExpression.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS_WITH_DEFAULT_PROXY(className, proxyClassName)						\
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS_COMMON(className)																		\
className* className::createDefaultInstance( Node* parent) { return proxyClassName::createDefaultInstance(parent);}	\

/**
 * Declares an attribute for a class inheriting from CompositeNode.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set a new node for this attibute.
 */
#define ATTRIBUTE(type, name, setMethodName)																									\
private:																																					\
		static ::Model::CompositeIndex name##Index;																							\
public:																																					\
		type* name() const { return static_cast< type* > (get(name##Index)); }														\
		void setMethodName(type* node) { set(name##Index, node); }																		\
private:																																					\

/**
 * Declares a private attribute for a class inheriting from CompositeNode.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set a new node for this attibute.
 */
#define PRIVATE_ATTRIBUTE(type, name, setMethodName)																						\
private:																																					\
		static ::Model::CompositeIndex name##Index;																							\
		type* name() { return static_cast< type* > (get(name##Index)); }																\
		void setMethodName(type* node) { set(name##Index, node); }																		\
private:																																					\

/**
 * INTERNAL
 *
 * Sets the value of a value attribute that might be NULL
 */
#define SET_ATTR_VAL(type, name)																													\
	type* node = static_cast<type*> (get(name##Index));																					\
	if (!node)																																			\
	{																																						\
		node = new type{};																															\
		set(name##Index, node);																														\
	}																																						\
	node->set(val);

/**
 * INTERNAL
 *
 * Sets the value of a value attribute defines inside of an Extension that might be NULL
 */
#define SET_EXTENSION_ATTR_VAL(type, name)																									\
	type* node = static_cast<type*> (self_->get(attr_[name##Index]));																	\
	if (!node)																																			\
	{																																						\
		node = new type{};																															\
		self_->set(attr_[name##Index], node);																									\
	}																																						\
	node->set(val);


/**
 * Declares an attribute for a class inheriting from CompositeNode. The attribute must have a get() and a set()
 * method.
 *
 * Defines three methods for this attribute:
 * 	- A method to return the node itself
 * 	- A method to return the simple value this node represents
 * 	- A method to set the simple value of this node.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set the value of this attibute.
 * @param valueType
 * 				The type of the simple value for this attribute.
 */
#define ATTRIBUTE_VALUE(type, name, setMethodName, valueType)																			\
private:																																					\
		static ::Model::CompositeIndex name##Index;																							\
public:																																					\
		type* name##Node() { return static_cast<type*> (get(name##Index)); }															\
		valueType name() const { return (static_cast<type*> (get(name##Index)))->get(); }										\
		void setMethodName(const valueType& val) { SET_ATTR_VAL(type, name) }														\
private:																																					\


/**
 * Declares a private attribute for a class inheriting from CompositeNode. The attribute must have a get() and a
 * set() method.
 *
 * Defines three methods for this attribute:
 * 	- A method to return the node itself
 * 	- A method to return the simple value this node represents
 * 	- A method to set the simple value of this node.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set the value of this attibute.
 * @param valueType
 * 				The type of the simple value for this attribute.
 */
#define PRIVATE_ATTRIBUTE_VALUE(type, name, setMethodName, valueType)																\
private:																																					\
		static ::Model::CompositeIndex name##Index;																							\
		type* name##Node() { return static_cast<type*> (get(name##Index)); }															\
		valueType name() const { return (static_cast<type*> (get(name##Index)))->get(); }										\
		void setMethodName(const valueType& val) { SET_ATTR_VAL(type, name) }														\
private:																																					\


/**
 * Declares an attribute for a class inheriting from CompositeNode. The attribute must have a get() and a set()
 * method.
 *
 * Defines three methods for this attribute:
 * 	- A method to return the node itself
 * 	- A method to return the simple value this node represents
 * 	- A method to set the simple value of this node.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set the value of this attibute.
 * @param valueType
 * 				The type of the simple value for this attribute.
 * @param returnValueType
 * 				The type of the returned value.
 */
#define ATTRIBUTE_VALUE_CUSTOM_RETURN(type, name, setMethodName, valueType, returnValueType)									\
private:																																					\
		static ::Model::CompositeIndex name##Index;																							\
public:																																					\
		type* name##Node() { return static_cast<type*> (get(name##Index)); }															\
		returnValueType name() const { return (static_cast<type*> (get(name##Index)))->get(); }								\
		void setMethodName(const valueType& val) { SET_ATTR_VAL(type, name) }														\
private:																																					\


 /**
  * Registers an attribute that was previously declared using an ATTRIBUTE macro or specified manually.
  *
  * @param className
  * 				The name of the class where this attribute was declared
  * @param attributeName
  * 				The name of the attribute
  * @param attributeType
  * 				The type of the attribute. This should only include the final type, without any namespace specifiers
  * @param partial
  * 				Whether this attribute should be partially loaded when reading from a persistent store
  * @param optional
  * 				Whether this attribute is optional
  * @param persistent
  * 				Whether this attribute should be persisted when saving the object to a persistent store.
  */
#define DEFINE_ATTRIBUTE(className, attributeName, attributeType, partial, optional, persistent)						\
::Model::CompositeIndex className::attributeName##Index = addAttributeToInitialRegistrationList_(							\
		attributeName##Index, #attributeName, #attributeType, partial, optional, persistent);

/**
 * Declares standard attributes and methods needed for each extension class.
 *
 * This macro should appear on the first line of the declaration of a class that is an extension for objects of type
 * CompositeNode or derived classes.
 *
 * @param className
 * 				The name of the extension lass that is being declared
 */
#define DECLARE_EXTENSION(className)																											\
private:																																					\
	::Model::CompositeNode* self_;																												\
	const QVector< ::Model::CompositeIndex >& attr_;																						\
																																							\
	static int extensionId_;																														\
	static QList< ::Model::Attribute >& attributesToRegister_();																		\
	static int addAttributeToRegister_( ::Model::Attribute attribute);																\
																																							\
public:																																					\
	className( ::Model::CompositeNode* self, const QVector< ::Model::CompositeIndex>& extensionAttributes);				\
																																							\
	static void registerExtension();																												\
	static int extensionId() { return extensionId_; }																						\
																																							\
	template <typename T> static void extendNode(QVector<Model::CompositeIndex>& extensionAttributes)							\
	{																																						\
		for (int i = 0; i<attributesToRegister_().size(); ++i)																			\
			extensionAttributes.append( T::registerNewAttribute(attributesToRegister_()[i]) );									\
	}																																						\
																																							\
private:

/**
 * Defines standard attributes and methods needed for each extension class.
 *
 * Use this in the beginning of the source (.cpp) file that belongs to the corresponding class declaration.
 *
 * @param className
 * 				The name of the extension lass that is being defined
 */
#define DEFINE_EXTENSION(className)																												\
int className::extensionId_ = -1;																												\
QList< ::Model::Attribute >& className::attributesToRegister_()																		\
{																																							\
	static QList< ::Model::Attribute > at;																										\
	return at;																																			\
}																																							\
																																							\
int className::addAttributeToRegister_( ::Model::Attribute attribute )																\
{																																							\
	attributesToRegister_().append( attribute );																								\
	return attributesToRegister_().size()-1;																									\
}																																							\
																																							\
className::className(::Model::CompositeNode* self, const QVector< ::Model::CompositeIndex>& extensionAttributes) :	\
	self_{self}, attr_{extensionAttributes} {}																								\
																																							\
void className::registerExtension()																												\
{																																							\
	extensionId_ = ::Model::CompositeNode::registerExtensionId();																		\
}																																							\

/**
 * Declares an attribute for a class providing an extension to CompositeNode.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set a new node for this attibute.
 */
#define EXTENSION_ATTRIBUTE(type, attributeName, setMethodName)																					\
private:																																					\
		static int attributeName##Index;																														\
public:																																					\
		type* attributeName() { return static_cast< type* > (self_->get(attr_[attributeName##Index])); }											\
		void setMethodName(type* node) { self_->set(attr_[attributeName##Index], node); }													\
private:																																					\

/**
 * Declares a private attribute for a class providing an extension to CompositeNode.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set a new node for this attibute.
 */
#define EXTENSION_PRIVATE_ATTRIBUTE(type, name, setMethodName)																			\
private:																																					\
		static int name##Index;																														\
																																							\
		type* name() { return static_cast< type* > (self_->get(attr_[name##Index])); }											\
		void setMethodName(type* node) { self_->set(attr_[name##Index], node); }													\
private:																																					\

/**
 * Declares an attribute for a class providing an extension to CompositeNode. The attribute must have a get() and
 * a set() method.
 *
 * Defines three methods for this attribute:
 * 	- A method to return the node itself
 * 	- A method to return the simple value this node represents
 * 	- A method to set the simple value of this node.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set the value of this attibute.
 * @param valueType
 * 				The type of the simple value for this attribute.
 */
#define EXTENSION_ATTRIBUTE_VALUE(type, attributeName, setMethodName, valueType)												\
private:																																					\
		static int attributeName##Index;																											\
public:																																					\
		type* attributeName##Node() const { return static_cast< type* > (self_->get(attr_[attributeName##Index])); }	\
		valueType attributeName() const { return (static_cast<type*> (self_->get(attr_[attributeName##Index])))->get(); }\
		void setMethodName(const valueType& val) { SET_EXTENSION_ATTR_VAL(type, attributeName) }								\
		void setMethodName##Node(type* node) { self_->set(attr_[attributeName##Index], node); }							\
private:																																					\

/**
 * Declares a private attribute for a class providing an extension to CompositeNode. The attribute must have a get()
 * and a set() method.
 *
 * Defines three methods for this attribute:
 * 	- A method to return the node itself
 * 	- A method to return the simple value this node represents
 * 	- A method to set the simple value of this node.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set the value of this attibute.
 * @param valueType
 * 				The type of the simple value for this attribute.
 */
#define EXTENSION_PRIVATE_ATTRIBUTE_VALUE(type, attributeName, setMethodName, valueType)													\
private:																																					\
		static int attributeName##Index;																														\
																																							\
		type* attributeName##Node() { return static_cast< type* > (self_->get(attr_[attributeName##Index])); }									\
		valueType attributeName() const { return (static_cast<type*> (self_->get(attr_[attributeName##Index])))->get(); }\
		void setMethodName(const valueType& val) { SET_EXTENSION_ATTR_VAL(type, attributeName) }											\
private:																																					\

/**
 * Declares an attribute for a class providing an extension to CompositeNode. The attribute must have a get() and
 * a set() method.
 *
 * Defines three methods for this attribute:
 * 	- A method to return the node itself
 * 	- A method to return the simple value this node represents
 * 	- A method to set the simple value of this node.
 *
 * @param type
 * 				The node type of the attribute
 * @param name
 * 				The name of the attribute. A method called name() will be created that can be used to access this
 * 				attribute.
 * @param setMethodName
 * 				The name of the set method that will set the value of this attibute.
 * @param valueType
 * 				The type of the simple value for this attribute.
 * @param returnValueType
 * 				The type of the returned value.
 */
#define EXTENSION_ATTRIBUTE_VALUE_CUSTOM_RETURN(type, attributeName, setMethodName, valueType, returnValueType)					\
private:																																					\
		static int attributeName##Index;																														\
public:																																					\
		type* attributeName##Node() { return static_cast< type* > (self_->get(attr_[attributeName##Index])); }									\
		returnValueType attributeName() const { return (static_cast<type*> \
																		(self_->get(attr_[attributeName##Index])))->get(); }				\
		void setMethodName(const valueType& val) { SET_EXTENSION_ATTR_VAL(type, attributeName) }											\
private:																																					\

 /**
  * Registers an attribute for an extension that was previously declared e.g using an EXTENSION_ATTRIBUTE macro.
  *
  * @param className
  * 				The name of the class where this attribute was declared
  * @param attributeName
  * 				The name of the attribute
  * @param attributeType
  * 				The type of the attribute. This should only include the final type, without any namespace specifiers
  * @param partial
  * 				Whether this attribute should be partially loaded when reading from a persistent store
  * @param optional
  * 				Whether this attribute is optional
  * @param persistent
  * 				Whether this attribute should be persisted when saving the object to a persistent store.
  */
#define DEFINE_EXTENSION_ATTRIBUTE(className, attributeName, attributeType, partial, optional, persistent)			\
int className::attributeName##Index = className::addAttributeToRegister_(															\
	::Model::Attribute{"_ext_" #className "_" #attributeName, #attributeType, optional, partial, persistent});
