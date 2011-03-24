/***********************************************************************************************************************
 * custommethodcall_api.h
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef CUSTOMMETHODCALL_API_H_
#define CUSTOMMETHODCALL_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(CUSTOMMETHODCALL_LIBRARY)
#  define CUSTOMMETHODCALL_API Q_DECL_EXPORT
#else
#  define CUSTOMMETHODCALL_API Q_DECL_IMPORT
#endif

#endif /* CUSTOMMETHODCALL_API_H_ */
