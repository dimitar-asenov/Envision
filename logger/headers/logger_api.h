/***********************************************************************************************************************
 * logger_api.h
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef LOGGER_API_H_
#define LOGGER_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(LOGGER_LIBRARY)
#  define LOGGER_API Q_DECL_EXPORT
#else
#  define LOGGER_API Q_DECL_IMPORT
#endif

#endif /* LOGGER_API_H_ */
