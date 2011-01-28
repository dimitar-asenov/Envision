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
		static const QString typeName_;																											\
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
		static const QString typeName_;																											\
																																							\
	public:																																				\
		className(::Model::Node* parent, ::Model::Model* model);																			\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint);\
		className(::Model::Node* parent, ::Model::Model* model, ::Model::AttributeChain& metaData);							\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint, ::Model::AttributeChain& metaData); \
																																							\
		static ::Model::AttributeChain& getMetaData();																						\
		static ::Model::ExtendableIndex registerNewAttribute(const QString &attributeName,										\
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent)					\
				{	return ExtendableNode::registerNewAttribute(getMetaData(), attributeName, attributeType, 					\
					canBePartiallyLoaded, isOptional, isPersistent );																		\
				}																																			\
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
const QString className::typeName_ = QString(#className);																				\
																																							\
const QString& className::typeName() const																									\
{																																							\
	return typeName_;																																	\
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
const QString className::typeName_ = QString(#className);																				\
																																							\
const QString& className::typeName() const																									\
{																																							\
	return typeName_;																																	\
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
 */
#define ATTRIBUTE(type, name)																														\
private:																																					\
		static ::Model::ExtendableIndex name##Index;																							\
public:																																					\
		type* name() { return static_cast<type*> (get(name##Index)); }																	\
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
		valueType name() { return (static_cast<type*> (get(name##Index)))->get(); }												\
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
		returnValueType name() { return (static_cast<type*> (get(name##Index)))->get(); }										\
		void setMethodName(const valueType& val) { (static_cast<type*> (get(name##Index)))->set(val); }						\
private:																																					\

/*********************************************************************************************************************/

#endif /* NODEMACROS_H_ */
