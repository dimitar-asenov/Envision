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
 * Declares standard constructors and standard static methods for registering the constructors.
 *
 * @param className
 * 			The name of the class being defined. This class must inherit from Node, directly or indirectly.
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
		className(Node* parent, Model* model);																									\
		className(Node *parent, NodeIdType id, PersistentStore &store, bool partialLoadHint);									\
																																							\
		QString getTypeName() const;																												\
																																							\
		static Node* createNew(Node* parent, Model* model);																				\
		static Node* createFromPersistence(Node *parent, NodeIdType id, PersistentStore &store, bool partialLoadHint);	\
																																							\
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
	className::className(Node* parent, Model* model) : parentName (parent, model) {}												\
																																							\
	className::className(Node *parent, NodeIdType id, PersistentStore &store, bool partialLoadHint)							\
		: parentName (parent, id, store, partialLoadHint) {}
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
	Node::registerNodeType(#className, createNew, createFromPersistence);															\
}																																							\
																																							\
Node* className::createNew(Node* parent, Model* model)																					\
{																																							\
	return new className(parent, model);																										\
}																																							\
																																							\
Node* className::createFromPersistence(Node *parent, NodeIdType id, PersistentStore &store, bool partialLoadHint)		\
{																																							\
	return new className(parent, id, store, partialLoadHint);																			\
}
/*********************************************************************************************************************/

#endif /* NODEMACROS_H_ */
