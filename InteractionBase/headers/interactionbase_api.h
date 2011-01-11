/***********************************************************************************************************************
 * interactionbase_api.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef INTERACTIONBASE_API_H_
#define INTERACTIONBASE_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(INTERACTIONBASE_LIBRARY)
#  define INTERACTIONBASE_API Q_DECL_EXPORT
#else
#  define INTERACTIONBASE_API Q_DECL_IMPORT
#endif

#endif /* INTERACTIONBASE_API_H_ */
