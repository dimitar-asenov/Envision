/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#pragma once

#include "../VisualizationException.h"
#include "StyleLoader.h"

namespace Visualization {
class Style;

extern QList<QMap<QString, Style*>*> styleSetMaps_;
void clearAllStyleSets();

template<typename T> class StyleSet
{
	public:
		StyleSet(const QString& classType);
		~StyleSet();

		typename T::StyleType* get(const QString& styleName = {});

	private:
		QMap<QString, typename T::StyleType*> styles_;

		/*
		 * This determines the folder inside the root 'styles' folder that will be used to look for this item's own
		 * directory. Typical values are "item", "shape", "layout" and "icon".
		 */
		QString classType_;
};

template<typename T> StyleSet<T>::StyleSet(const QString& classType) : classType_{classType}
{
	styleSetMaps_.append(reinterpret_cast<QMap<QString, Style*>*>(&styles_));
}
template<typename T> StyleSet<T>::~StyleSet() {}


template<typename T> typename T::StyleType* StyleSet<T>::get(const QString& styleName)
{
	QString name(styleName);
	if (name.isEmpty()) name = "default";

	typename T::StyleType* style = styles_.value(name, nullptr);
	if (style) return style;

	style = StyleLoader{}.loadStyle<typename T::StyleType>(classType_ + "/" + T::typeNameStatic(), name);
	if (style)
	{
		styles_.insert(name, style);
		return style;
	}
	else throw VisualizationException{"Could not find the style '" + classType_ + "/" + T::typeNameStatic()
					+ "/" + styleName + "'"};
}

}
