/***********************************************************************************************************************
 * controlflowvisualization_api.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef CONTROLFLOWVISUALIZATION_API_H_
#define CONTROLFLOWVISUALIZATION_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(CONTROLFLOWVISUALIZATION_LIBRARY)
#  define CONTROLFLOWVISUALIZATION_API Q_DECL_EXPORT
#else
#  define CONTROLFLOWVISUALIZATION_API Q_DECL_IMPORT
#endif

#endif /* CONTROLFLOWVISUALIZATION_API_H_ */
