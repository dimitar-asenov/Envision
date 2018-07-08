#include "CppImportLightException.h"

namespace CppImportLight {

CppImportLightException::CppImportLightException(const QString& message) :
	Core::EnvisionException(message)
{
}

const QString& CppImportLightException::name() const
{
	static QString ename("CppImportLightException");
	return ename;
}

}
