#pragma once

#include "PLUGINNAME_LOWERCASE_api.h"
#include "Core/src/EnvisionException.h"

namespace NAMESPACE {

class PLUGINNAME_UPPERCASE_API PLUGINNAMEException : public Core::EnvisionException
{
		public:
			PLUGINNAMEException(const QString& message);
			const QString& name() const;
};

}
