/***********************************************************************************************************************
 * helloworld_api.h
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef HELLOWORLD_API_H_
#define HELLOWORLD_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(HELLOWORLD_LIBRARY)
#  define HELLOWORLD_API Q_DECL_EXPORT
#else
#  define HELLOWORLD_API Q_DECL_IMPORT
#endif

#endif /* HELLOWORLD_API_H_ */
