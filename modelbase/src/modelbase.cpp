/***********************************************************************************************************************
 * modelbase.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"

Q_EXPORT_PLUGIN2( modelbase, Model::ModelBase )

namespace Model {

bool ModelBase::initialize(Envision::EnvisionManager&)
{
	return true;
}

void ModelBase::selfTest(QString)
{
}

}
