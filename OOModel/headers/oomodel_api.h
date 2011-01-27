/***********************************************************************************************************************
 * oomodel_api.h
 *
 *  Created on: Jan 27, 2011
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef OOMODEL_API_H_
#define OOMODEL_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(OOMODEL_LIBRARY)
#  define OOMODEL_API Q_DECL_EXPORT
#else
#  define OOMODEL_API Q_DECL_IMPORT
#endif

#endif /* OOMODEL_API_H_ */
