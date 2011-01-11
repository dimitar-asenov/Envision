/***********************************************************************************************************************
 * PLUGINNAME_LOWERCASE_api.h
 *
 *  Created on: DATE
 *      Author: AUTHOR
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef PLUGINNAME_UPPERCASE_API_H_
#define PLUGINNAME_UPPERCASE_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(PLUGINNAME_UPPERCASE_LIBRARY)
#  define PLUGINNAME_UPPERCASE_API Q_DECL_EXPORT
#else
#  define PLUGINNAME_UPPERCASE_API Q_DECL_IMPORT
#endif

#endif /* PLUGINNAME_UPPERCASE_API_H_ */
