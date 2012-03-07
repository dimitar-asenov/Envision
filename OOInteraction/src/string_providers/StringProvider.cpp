/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

/*
 * StringProvider.cpp
 *
 *  Created on: Feb 14, 2012
 *      Author: Dimitar Asenov
 */

#include "string_providers/StringProvider.h"
#include "string_components/StringComponents.h"

#include "VisualizationBase/headers/items/Item.h"
#include "ModelBase/headers/adapter/AdapterManager.h"
#include "Core/headers/global.h"

namespace OOInteraction {

StringProvider::~StringProvider()
{
}

QStringList StringProvider::components(Model::Node* node)
{
	if (!node) return QStringList();

	QStringList result;
	StringComponents* sc = Model::AdapterManager::adapt<StringComponents>(node);
	if (sc)
	{
		result = sc->components();
		SAFE_DELETE(sc);
	}

	return result;
}

QString StringProvider::stringFromComponenets(Model::Node* node)
{
	if (!node) return QString();

	QString result;
	StringComponents* sc = Model::AdapterManager::adapt<StringComponents>(node);
	if (sc)
	{
		result = sc->components().join("");
		SAFE_DELETE(sc);
	}

	return result;
}

QString StringProvider::stringFromComponenets(Visualization::Item* item)
{
	if (!item) return QString();
	return stringFromComponenets(item->node());
}

QString StringProvider::stringFromStringProvider(Visualization::Item* item)
{
	if (!item) return QString();

	QString result;
	StringProvider* sp = Model::AdapterManager::adapt<StringProvider>(item);
	if (sp)
	{
		result = sp->string();
		SAFE_DELETE(sp);
	}

	return result;
}

bool StringProvider::isIndivisible()
{
	return false;
}

} /* namespace OOInteraction */
