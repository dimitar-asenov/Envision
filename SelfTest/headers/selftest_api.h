/***********************************************************************************************************************
 * selftest_api.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 *
 * This header defines the import/export macro which should be used when this plug-in wants to export classes or
 * functions to other classes.
 **********************************************************************************************************************/

#ifndef SELFTEST_API_H_
#define SELFTEST_API_H_

#include <QtCore/QtGlobal>

// This should be defined in the project file of the plug-in that exports symbols
#if defined(SELFTEST_LIBRARY)
#  define SELFTEST_API Q_DECL_EXPORT
#else
#  define SELFTEST_API Q_DECL_IMPORT
#endif

#endif /* SELFTEST_API_H_ */
