/***********************************************************************************************************************
 * attributeMacros.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ATTRIBUTEMACROS_H_
#define ATTRIBUTEMACROS_H_

/**
 * Declares a 'name' attribute of type Model::Text.
 *
 * The attribute node can be accessed via nameNode()
 * The name string is accessible via name()
 * A new value can be set via setName()
 */
#define ATTRIBUTE_OOP_NAME																															\
	ATTRIBUTE_VALUE_CUSTOM_RETURN(::Model::Text, name, setName, QString, const QString&)

/*********************************************************************************************************************/

/**
 * Declares a 'visibility' attribute of type OOModel::Visibility.
 *
 * The attribute node can be accessed via visibilityNode()
 * The visibility type is accessible via visibility()
 * A new value can be set via setVisibility()
 */
#define ATTRIBUTE_OOP_VISIBILITY																													\
	ATTRIBUTE_VALUE(::OOModel::Visibility, visibility, setVisibility, ::OOModel::Visibility::VisibilityType)

/*********************************************************************************************************************/

/**
 * Declares a 'static' attribute of type OOModel::Static.
 *
 * The attribute node can be accessed via statNode()
 * The visibility type is accessible via stat()
 * A new value can be set via setStat()
 */
#define ATTRIBUTE_OOP_STATIC																											\
	ATTRIBUTE_VALUE(::OOModel::Static, stat, setStat, ::OOModel::Static::StaticType)

/*********************************************************************************************************************/

#endif /* ATTRIBUTEMACROS_H_ */
