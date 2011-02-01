/***********************************************************************************************************************
 * nodeMacros.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 *
 *  This file contains a few macros that can be used when creating new classes derived from Node. Using these macros
 *  might help reduce the size of boilerplate code that needs to be written.
 **********************************************************************************************************************/

#ifndef NODEMACROS_H_
#define NODEMACROS_H_

#include "ModelBase/headers/nodes/Extendable/Attribute.h"
#include "ModelBase/headers/nodes/Extendable/ExtendableIndex.h"

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QPair>

/**
 * Declares standard constructors and standard static methods for registering the constructors of a class inheriting
 * Node.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from Node, directly or indirectly. For
 * 			classes directly inheriting ExtendableNode use the macro EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS.
 *
 * This macro declares a static method "registerNodeConstructors" which should be called during the initialization of
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
	private:																																				\
		static int typeId_;																															\
																																							\
	public:																																				\
		className(::Model::Node* parent, ::Model::Model* model);																			\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint);\
																																							\
		virtual const QString& typeName() const;																								\
		virtual int typeId() const;																												\
		static const QString& typeNameStatic();																								\
		static int typeIdStatic();																													\
		static void registerNodeType();																											\
																																							\
	private:
/*********************************************************************************************************************/

/**
 * Declares standard constructors and standard static methods for registering the constructors of a class inheriting
 * ExtendableNode.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from ExtendableNode, directly or indirectly.
 *
 * This macro declares a static method "registerNodeConstructors" which should be called during the initialization of
 * the plug-in where this node is defined. This will assure that the new node type's constructors are properly
 * registered.
 *
 * This macro should appear as the first line after the class declaration e.g. :
 *
 * class MyNewNode : public ExtendableNode
 * {
 * 	EXTENDABLENODE_DECLARE_STANDARD_METHODS( MyNewNode )
 */
#define EXTENDABLENODE_DECLARE_STANDARD_METHODS(className)																				\
	private:																																				\
		static int typeId_;																															\
																																							\
	public:																																				\
		className(::Model::Node* parent, ::Model::Model* model);																			\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint);\
		className(::Model::Node* parent, ::Model::Model* model, ::Model::AttributeChain& metaData);							\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint, ::Model::AttributeChain& metaData); \
																																							\
		static void init();																															\
																																							\
		static ::Model::AttributeChain& getMetaData();																						\
		static ::Model::ExtendableIndex registerNewAttribute(const QString &attributeName,										\
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent);				\
																																							\
		static ::Model::ExtendableIndex registerNewAttribute(const ::Model::Attribute& attribute);							\
																																							\
		virtual const QString& typeName() const;																								\
		virtual int typeId() const;																												\
		static const QString& typeNameStatic();																								\
		static int typeIdStatic();																													\
		static void registerNodeType();																											\
																																							\
	private:																																				\
		static QList<QPair< ::Model::ExtendableIndex&, ::Model::Attribute> >& attributesToRegisterAtInitialization_();	\
		static ::Model::ExtendableIndex addAttributeToInitialRegistrationList_														\
				(::Model::ExtendableIndex& index, const QString &attributeName, const QString &attributeType, 				\
					bool canBePartiallyLoaded, bool isOptional, bool isPersistent);													\

/*********************************************************************************************************************/

/**
 * Defines standard empty constructors for a new Node type which just call their parent constructors.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from parentName.
 *
 * @param parentName
 * 			The name of the direct parent class. This class must be or inherit from from Node, directly or indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define NODE_DEFINE_EMPTY_CONSTRUCTORS(className, parentName)																			\
	className::className(::Model::Node* parent, ::Model::Model* model) : parentName (parent, model) {}						\
																																							\
	className::className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint)\
		: parentName (parent, id, store, partialLoadHint) {}
/*********************************************************************************************************************/

/**
 * Defines standard empty constructors for a new Node type which just call their parent constructors.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from parentName.
 *
 * @param parentName
 * 			The name of the direct parent class. This class must be or inherit from from ExtendableNode, directly or
 * 			indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(className, parentName)																\
	className::className(::Model::Node* parent, ::Model::Model* model)																\
		: parentName (parent, model, className::getMetaData()) {}																		\
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	\
	className::className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint)\
		: parentName (parent, id, store, partialLoadHint, className::getMetaData()) {}											\
		  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	   \
	className::className(::Model::Node* parent, ::Model::Model* model, ::Model::AttributeChain& metaData)					\
		: parentName (parent, model, metaData) {}																								\
																																							\
	className::className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint, ::Model::AttributeChain& metaData)\
		: parentName (parent, id, store, partialLoadHint, metaData) {}
/*********************************************************************************************************************/

/**
 * Defines standard static methods that register the new Node type's constructors and a virtual getTypeName method
 * that returns the name of this class.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from from Node, directly or indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define NODE_DEFINE_TYPE_REGISTRATION_METHODS(className)																					\
int className::typeId_ = -1; /* This must be set to the result of Node::registerNodeType */									\
																																							\
const QString& className::typeName() const																									\
{																																							\
	return typeNameStatic();																														\
}																																							\
																																							\
int className::typeId()	const																														\
{																																							\
	return typeId_;																																	\
}																																							\
int className::typeIdStatic()																														\
{																																							\
	return typeId_;																																	\
}																																							\
const QString& className::typeNameStatic()																									\
{																																							\
	static QString typeName_(#className);																										\
	return typeName_;																																	\
}																																							\
																																							\
void className::registerNodeType()																												\
{																																							\
	typeId_ = Node::registerNodeType(#className, ::Model::createNewNode< className >, ::Model::createNodeFromPersistence< className >);\
}
/*********************************************************************************************************************/

/**
 * Defines standard static methods that register the new Node type's constructors and a virtual getTypeName method
 * that returns the name of this class.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from from ExtendableNode, directly or
 * 			indirectly.
 *
 * Use this macro in the .cpp file that defines the new Node type.
 */
#define EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(className, parentName)													\
int className::typeId_ = -1; /* This must be set to the result of Node::registerNodeType */									\
																																							\
const QString& className::typeName() const																									\
{																																							\
	return typeNameStatic();																														\
}																																							\
																																							\
int className::typeId()	const																														\
{																																							\
	return typeId_;																																	\
}																																							\
int className::typeIdStatic()																														\
{																																							\
	return typeId_;																																	\
}																																							\
const QString& className::typeNameStatic()																									\
{																																							\
	static QString typeName_(#className);																										\
	return typeName_;																																	\
}																																							\
																																							\
void className::registerNodeType()																												\
{																																							\
	className::getMetaData().setParent(&parentName::getMetaData());																	\
	typeId_ = Node::registerNodeType(#className, ::Model::createNewNode< className >, ::Model::createNodeFromPersistence< className >);\
}																																							\
																																							\
::Model::AttributeChain& className::getMetaData()																							\
{																																							\
	static ::Model::AttributeChain descriptions;																								\
	return descriptions;																																\
}																																							\
																																							\
QList<QPair< ::Model::ExtendableIndex&, ::Model::Attribute> >& className::attributesToRegisterAtInitialization_()		\
{																																							\
	static QList<QPair< ::Model::ExtendableIndex&, ::Model::Attribute> > a;															\
	return a;																																			\
}																																							\
																																							\
::Model::ExtendableIndex className::registerNewAttribute(const QString &attributeName,											\
	const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)								\
{																																							\
	return ExtendableNode::registerNewAttribute(getMetaData(), attributeName, attributeType, 									\
					canBePartiallyLoaded, isOptional, isPersistent );																		\
}																																							\
																																							\
::Model::ExtendableIndex className::registerNewAttribute(const ::Model::Attribute& attribute)								\
{																																							\
	return ExtendableNode::registerNewAttribute(getMetaData(), attribute);															\
}																																							\
																																							\
::Model::ExtendableIndex className::addAttributeToInitialRegistrationList_ (::Model::ExtendableIndex& index,			\
	const QString &attributeName, const QString &attributeType, bool canBePartiallyLoaded, bool isOptional,				\
			 bool isPersistent)																														\
{																																							\
	attributesToRegisterAtInitialization_().append(QPair< ::Model::ExtendableIndex&, ::Model::Attribute>(index,			\
			::Model::Attribute(attributeName, attributeType, isOptional, canBePartiallyLoaded, isPersistent)));			\
	return ::Model::ExtendableIndex();																											\
}																																							\
																																							\
void className::init()																																\
{																																							\
	registerNodeType();																																\
	for (int i = 0; i<attributesToRegisterAtInitialization_().size(); ++i)															\
		attributesToRegisterAtInitialization_().at(i).first =																				\
				registerNewAttribute(attributesToRegisterAtInitialization_().at(i).second);										\
}
/*********************************************************************************************************************/

/**
 * Declares an attribute for a class inheriting from ExtendableNode or for a class providing an extension to an
 * ExtendableNode.
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
		static ::Model::ExtendableIndex name##Index;																							\
public:																																					\
		type* name() { return static_cast< type* > (get(name##Index)); }																\
		template <class T> T* setMethodName() { return set<T>(name##Index,T::typeNameStatic()); }								\
private:																																					\

/*********************************************************************************************************************/

/**
 * Declares a private attribute for a class inheriting from ExtendableNode or for a class providing an extension to an
 * ExtendableNode.
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
		static ::Model::ExtendableIndex name##Index;																							\
		type* name() { return static_cast< type* > (get(name##Index)); }																\
		template <class T> T* setMethodName() { return set<T>(name##Index,T::typeNameStatic()); }								\
private:																																					\

/*********************************************************************************************************************/

/**
 * Declares an attribute for a class inheriting from ExtendableNode or for a class providing an extension to an
 * ExtendableNode. The attribute must have a get() and a set() method.
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
		static ::Model::ExtendableIndex name##Index;																							\
public:																																					\
		type* name##Node() { return static_cast<type*> (get(name##Index)); }															\
		valueType name() const { return (static_cast<type*> (get(name##Index)))->get(); }										\
		void setMethodName(const valueType& val) { (static_cast<type*> (get(name##Index)))->set(val); }						\
private:																																					\

/**
 * Declares a private attribute for a class inheriting from ExtendableNode or for a class providing an extension to an
 * ExtendableNode. The attribute must have a get() and a set() method.
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
		static ::Model::ExtendableIndex name##Index;																							\
		type* name##Node() { return static_cast<type*> (get(name##Index)); }															\
		valueType name() const { return (static_cast<type*> (get(name##Index)))->get(); }										\
		void setMethodName(const valueType& val) { (static_cast<type*> (get(name##Index)))->set(val); }						\
private:																																					\

/*********************************************************************************************************************/

/**
 * Declares an attribute for a class inheriting from ExtendableNode or for a class providing an extension to an
 * ExtendableNode. The attribute must have a get() and a set() method.
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
		static ::Model::ExtendableIndex name##Index;																							\
public:																																					\
		type* name##Node() { return static_cast<type*> (get(name##Index)); }															\
		returnValueType name() const { return (static_cast<type*> (get(name##Index)))->get(); }								\
		void setMethodName(const valueType& val) { (static_cast<type*> (get(name##Index)))->set(val); }						\
private:																																					\

/*********************************************************************************************************************/

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
 #define REGISTER_ATTRIBUTE(className, attributeName, attributeType, partial, optional, persistent)						\
::Model::ExtendableIndex className::attributeName##Index = addAttributeToInitialRegistrationList_(							\
		attributeName##Index, #attributeName, #attributeType, partial, optional, persistent);

#endif /* NODEMACROS_H_ */
