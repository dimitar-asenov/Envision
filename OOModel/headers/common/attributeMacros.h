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
 * A new name can be set via setName()
 */
#define ATTRIBUTE_OOP_NAME																															\
	ATTRIBUTE_VALUE_CUSTOM_RETURN(Model::Text, name, setName, QString, const QString&)

/*********************************************************************************************************************/

#endif /* ATTRIBUTEMACROS_H_ */
