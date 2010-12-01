/***********************************************************************************************************************
 * helloworld_api.h
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef MODELBASE_API_H_
#define MODELBASE_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(MODELBASE_LIBRARY)
#  define MODELBASE_API Q_DECL_EXPORT
#else
#  define MODELBASE_API Q_DECL_IMPORT
#endif

#endif /* MODELBASE_API_H_ */
