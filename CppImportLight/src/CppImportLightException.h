#pragma once

#include "cppimportlight_api.h"
#include "Core/src/EnvisionException.h"

namespace CppImportLight {

class CPPIMPORTLIGHT_API CppImportLightException : public Core::EnvisionException
{
		public:
			CppImportLightException(const QString& message);
			const QString& name() const;
};

}
