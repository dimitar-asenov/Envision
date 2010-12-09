/***********************************************************************************************************************
 * visualizationbase_api.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef VISUALIZATIONBASE_API_H_
#define VISUALIZATIONBASE_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(VISUALIZATIONBASE_LIBRARY)
#define VISUALIZATIONBASE_API Q_DECL_EXPORT
#else
#define VISUALIZATIONBASE_API Q_DECL_IMPORT
#endif

#endif /* VISUALIZATIONBASE_API_H_ */
