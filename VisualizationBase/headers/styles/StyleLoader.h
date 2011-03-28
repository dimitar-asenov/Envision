/***********************************************************************************************************************
 * StyleLoader.h
 *
 *  Created on: Mar 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STYLELOADER_H_
#define STYLELOADER_H_

#include "../visualizationbase_api.h"
#include "StyleNode.h"

#include "Core/headers/global.h"

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QtCore/QVector>

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

		template <class T> T* loadStyle(const QString& path, const QString& styleName);

		bool hasProperty(const QString& name);

		void load(const QString& propertyName, int& value);
		void load(const QString& propertyName, bool& value);
		void load(const QString& propertyName, QString& value);
		void load(const QString& propertyName, double& value);
		template <class T> void load(const QString& propertyName, T& value);

	private:
		QList< QString > nodePath;
		StyleNode* rootStyleNode;

		QString getProperty(const QString& name);

		template <class T> void loadComposite(T& value);

		void loadComposite(QPointF& value);
		void loadComposite(QColor& value);
		void loadComposite(QPen& value);
		void loadComposite(QFont& value);
		void loadComposite(QBrush& value);
		void loadComposite(QLinearGradient& value);
		void loadComposite(QRadialGradient& value);
		template <typename T> void loadComposite(QVector<T>& value);
		template <typename T> void loadComposite(QList<T>& value);
		template <typename F, typename S> void loadComposite(QPair<F,S>& value);
};

template <class T> T* StyleLoader::loadStyle(const QString& path, const QString& styleName)
{
	rootStyleNode = new StyleNode(styleName, path);

	T* style = new T();
	load("style", *style);

	SAFE_DELETE(rootStyleNode);

	return style;
}

template <class T> void StyleLoader::load(const QString& propertyName, T& value)
{
	nodePath.append(propertyName);
	loadComposite(value);
	nodePath.removeLast();
}

template <class T> inline void StyleLoader::loadComposite(T& value) { value.load(*this); }

template <class T> void StyleLoader::loadComposite(QVector<T>& value)
{
	QList<T> list;
	loadComposite(list);
	value = list.toVector();
}

template <typename T> void StyleLoader::loadComposite(QList<T>& value)
{
	int length;
	load("length", length);

	value.clear();
	for(int i = 0; i< length; ++i)
	{
		T element;
		load("e"+QString::number(i), element);
		value.append(element);
	}
}

template <typename F, typename S> void StyleLoader::loadComposite(QPair<F,S>& value)
{
	F first;
	S second;
	load("first", first);
	load("second", second);
	value = QPair<F,S>(first, second);
}

}

#endif /* STYLELOADER_H_ */
