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
 * 	NODE_DECLARE_STANDARD_CONSTRUCTORS( MyNewNode )
 */
#define NODE_DECLARE_STANDARD_CONSTRUCTORS(className)																						\
	public:																																				\
		className(::Model::Node* parent, ::Model::Model* model);																			\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint);\
																																							\
		QString getTypeName() const;																												\
		static void registerNodeConstructors();																								\
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
 * 	EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS( MyNewNode )
 */
#define EXTENDABLENODE_DECLARE_STANDARD_CONSTRUCTORS(className)																		\
	public:																																				\
		className(::Model::Node* parent, ::Model::Model* model, ::Model::AttributeChain& metaData);							\
		className(::Model::Node *parent, ::Model::NodeIdType id, ::Model::PersistentStore &store, bool partialLoadHint, ::Model::AttributeChain& metaData); \
																																							\
		QString getTypeName() const;																												\
		static void registerNodeConstructors();																								\
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
QString className::getTypeName()	const																											\
{																																							\
	return #className;																																\
}																																							\
																																							\
void className::registerNodeConstructors()																									\
{																																							\
	Node::registerNodeType(#className, ::Model::createNewNode< className >, ::Model::createNodeFromPersistence< className >);\
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
#define EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(className)																	\
QString className::getTypeName()	const																											\
{																																							\
	return #className;																																\
}																																							\
																																							\
void className::registerNodeConstructors()																									\
{																																							\
	Node::registerNodeType(#className, ::Model::createNewExtendableNode< className >, ::Model::createExtendableNodeFromPersistence< className >);\
}
/*********************************************************************************************************************/

#endif /* NODEMACROS_H_ */
