#include "PLUGINNAMEException.h"

namespace NAMESPACE {

PLUGINNAMEException::PLUGINNAMEException(const QString& message) :
	Core::EnvisionException(message)
{
}

const QString& PLUGINNAMEException::name() const
{
	static QString ename("PLUGINNAMEException");
	return ename;
}

}
