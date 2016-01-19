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

#include "../visualizationbase_api.h"
#include "StyleNode.h"

class QColor;
class QPen;
class QFont;
class QBrush;
class QLinearGradient;
class QRadialGradient;
class QPointF;

namespace Visualization {

class StyleNode;

class VISUALIZATIONBASE_API StyleLoader
{
	public:
		StyleLoader();
		~StyleLoader();

		template <typename T> T* loadStyle(const QString& path, const QString& styleName);

		bool hasProperty(const QString& name);

		void load(const QString& propertyName, int& value);
		void load(const QString& propertyName, bool& value);
		void load(const QString& propertyName, QString& value);
		void load(const QString& propertyName, double& value);
		void load(const QString& propertyName, QColor& value);
		template <typename T> void load(const QString& propertyName, T& value);

	private:
		QList< QString > nodePath;
		StyleNode* rootStyleNode;

		QString getProperty(const QString& name);

		template <typename T> void loadComposite(T& value);

		void loadComposite(QPointF& value);
		void loadComposite(QPen& value);
		void loadComposite(QFont& value);
		void loadComposite(QBrush& value);
		void loadComposite(QLinearGradient& value);
		void loadComposite(QRadialGradient& value);
		template <typename T> void loadComposite(QVector<T>& value);
		template <typename T> void loadComposite(QList<T>& value);
		template <typename F, typename S> void loadComposite(QPair<F, S>& value);
};

template <typename T> T* StyleLoader::loadStyle(const QString& path, const QString& styleName)
{
	rootStyleNode = new StyleNode{styleName, path};

	QString styleRootNodeName = "style";
	auto subNodeIndex = styleName.indexOf('/');
	if (subNodeIndex > 0) styleRootNodeName = styleName.mid(subNodeIndex+1);

	T* style = new T{};
	load(styleRootNodeName, *style);

	SAFE_DELETE(rootStyleNode);

	return style;
}

template <typename T> void StyleLoader::load(const QString& propertyName, T& value)
{
	nodePath.append(propertyName);
	loadComposite(value);
	nodePath.removeLast();
}

template <typename T> inline void StyleLoader::loadComposite(T& value) { value.load(*this); }

template <typename T> void StyleLoader::loadComposite(QVector<T>& value)
{
	QList<T> list;
	loadComposite(list);
	value = list.toVector();
}

template <typename T> void StyleLoader::loadComposite(QList<T>& value)
{
	value.clear();
	for (int i = 0; ; ++i)
	{
		auto elementName = "e"+QString::number(i);

		if (hasProperty(elementName))
		{
			T element;
			load(elementName, element);
			value.append(element);
		}
		else break;
	}
}

template <typename F, typename S> void StyleLoader::loadComposite(QPair<F, S>& value)
{
	F first;
	S second;
	load("first", first);
	load("second", second);
	value = QPair<F, S>(first, second);
}

}
