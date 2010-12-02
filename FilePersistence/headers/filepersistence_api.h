/***********************************************************************************************************************
 * filepersistence_api.h
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef FILEPERSISTENCE_API_H_
#define FILEPERSISTENCE_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(FILEPERSISTENCE_LIBRARY)
#  define FILEPERSISTENCE_API Q_DECL_EXPORT
#else
#  define FILEPERSISTENCE_API Q_DECL_IMPORT
#endif

#endif /* FILEPERSISTENCE_API_H_ */
