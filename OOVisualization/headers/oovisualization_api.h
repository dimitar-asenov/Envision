/***********************************************************************************************************************
 * oovisualization_api.h
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef OOVISUALIZATION_API_H_
#define OOVISUALIZATION_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(OOVISUALIZATION_LIBRARY)
#  define OOVISUALIZATION_API Q_DECL_EXPORT
#else
#  define OOVISUALIZATION_API Q_DECL_IMPORT
#endif

#endif /* OOVISUALIZATION_API_H_ */
