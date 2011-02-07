/***********************************************************************************************************************
 * Styles.h
 *
 *  Created on: Dec 17, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STYLES_H_
#define STYLES_H_

#include "visualizationbase_api.h"
#include "VisualizationException.h"

#include "StyleNode.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"

#include "Core/headers/global.h"
#include <QtXml/QDomDocument>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QMap>
#include <QtCore/QDir>
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

class VISUALIZATIONBASE_API Styles
{
	private:
		static const QString STYLES_DIRECTORY_NAME;

		static QMutex accessMutex;
		static QList< QString > nodePath;
		static StyleNode* root;

		static QString getProperty(const QString& name);

		template <class T> static QMap<QString, typename T::StyleType*>& itemStyles();
		template <class T> static QMap<QString, typename T::StyleType*>& layoutStyles();
		template <class T> static QMap<QString, typename T::StyleType*>& shapeStyles();
		template <class T> static QMap<QString, typename T::StyleType*>& iconStyles();

		template <class T> static typename T::StyleType* loadStyle(const QString& objectClass, const QString& styleName);

		static void loadQPointF(QPointF& value);
		static void loadQColor(QColor& value);
		static void loadQPen(QPen& value);
		static void loadQFont(QFont& value);
		static void loadQBrush(QBrush& value);
		static void loadQLinearGradient(QLinearGradient& value);
		static void loadQRadialGradient(QRadialGradient& value);

	public:
		static void load(const QString& propertyName, int& value);
		static void load(const QString& propertyName, bool& value);
		static void load(const QString& propertyName, QString& value);
		static void load(const QString& propertyName, double& value);
		template <class T> static void load(const QString& propertyName, T& value);
		template <class T> static void load(T& value);
		template <typename T> static void load(QVector<T>& value);
		template <typename T> static void load(QList<T>& value);
		template <typename F, typename S> static void load(QPair<F,S>& value);

		template <class T> static typename T::StyleType* item(const QString& styleName = QString());
		template <class T> static typename T::StyleType* layout(const QString& styleName = QString());
		template <class T> static typename T::StyleType* shape(const QString& styleName = QString());
		template <class T> static typename T::StyleType* icon(const QString& styleName = QString());
};

template<class T> QMap<QString, typename T::StyleType*>& Styles::itemStyles()
{
	static QMap<QString, typename T::StyleType*> styles;
	return styles;
}

template<class T> QMap<QString, typename T::StyleType*>& Styles::layoutStyles()
{
	static QMap<QString, typename T::StyleType*> styles;
	return styles;
}

template<class T> QMap<QString, typename T::StyleType*>& Styles::shapeStyles()
{
	static QMap<QString, typename T::StyleType*> styles;
	return styles;
}

template<class T> QMap<QString, typename T::StyleType*>& Styles::iconStyles()
{
	static QMap<QString, typename T::StyleType*> styles;
	return styles;
}

template<class T> typename T::StyleType* Styles::item(const QString& styleName)
{
	if ( styleName.isNull() ) return T::StyleType::getDefault();
	if ( itemStyles<T> ().contains(styleName) ) return itemStyles<T> ().value(styleName);
	else
	{
		typename T::StyleType* style = loadStyle<T> ("item", styleName);
		if ( style )
		{
			itemStyles<T> ().insert(styleName, style);
			return style;
		}
	}

	throw VisualizationException("Could not find the item style '" + styleName + "'");
}

template<class T> typename T::StyleType* Styles::layout(const QString& styleName)
{
	if ( styleName.isNull() ) return T::StyleType::getDefault();
	if ( layoutStyles<T> ().contains(styleName) ) return layoutStyles<T> ().value(styleName);
	else
	{
		typename T::StyleType* style = loadStyle<T> ("layout", styleName);
		if ( style )
		{
			layoutStyles<T> ().insert(styleName, style);
			return style;
		}
	}

	throw VisualizationException("Could not find the layout style '" + styleName + "'");
}

template<class T> typename T::StyleType* Styles::shape(const QString& styleName)
{
	if ( styleName.isNull() ) return T::StyleType::getDefault();
	if ( shapeStyles<T> ().contains(styleName) ) return shapeStyles<T> ().value(styleName);
	else
	{
		typename T::StyleType* style = loadStyle<T> ("shape", styleName);
		if ( style )
		{
			shapeStyles<T> ().insert(styleName, style);
			return style;
		}
	}

	throw VisualizationException("Could not find the shape style '" + styleName + "'");
}

template<class T> typename T::StyleType* Styles::icon(const QString& styleName)
{
	if ( styleName.isNull() ) return T::StyleType::getDefault();
	if ( iconStyles<T> ().contains(styleName) ) return iconStyles<T> ().value(styleName);
	else
	{
		typename T::StyleType* style = loadStyle<T> ("icon", styleName);
		if ( style )
		{
			iconStyles<T> ().insert(styleName, style);
			return style;
		}
	}

	throw VisualizationException("Could not find the icon style '" + styleName + "'");
}

template<class T> typename T::StyleType* Styles::loadStyle(const QString& objectClass, const QString& styleName)
{
	QMutexLocker locker(&accessMutex);

	StyleNode::setBaseFolder(STYLES_DIRECTORY_NAME);
	root = new StyleNode(styleName, objectClass + "/" + T::className());

	typename T::StyleType* style = new typename T::StyleType();
	load("style", *style);

	SAFE_DELETE(root);

	return style;
}

template <class T> void Styles::load(const QString& propertyName, T& value)
{
	nodePath.append(propertyName);
	load(value);
	nodePath.removeLast();
}

template <class T> inline void Styles::load(T& value) { value.load(); }
template <> inline void Styles::load<QPointF>(QPointF& value) { loadQPointF(value); }
template <> inline void Styles::load<QPen>(QPen& value) { loadQPen(value); }
template <> inline void Styles::load<QColor>(QColor& value) { loadQColor(value); }
template <> inline void Styles::load<QFont>(QFont& value) { loadQFont(value); }
template <> inline void Styles::load<QBrush>(QBrush& value) { loadQBrush(value); }
template <> inline void Styles::load<QLinearGradient>(QLinearGradient& value) { loadQLinearGradient(value); }
template <> inline void Styles::load<QRadialGradient>(QRadialGradient& value) { loadQRadialGradient(value); }

template <class T> void Styles::load(QVector<T>& value)
{
	QList<T> list;
	load(list);
	value = list.toVector();
}

template <typename T> void Styles::load(QList<T>& value)
{
	int length;
	Styles::load("length", length);

	value.clear();
	for(int i = 0; i< length; ++i)
	{
		T element;
		Styles::load("e"+QString::number(i), element);
		value.append(element);
	}
}

template <typename F, typename S> void Styles::load(QPair<F,S>& value)
{
	F first;
	S second;
	Styles::load("first", first);
	Styles::load("second", second);
	value = QPair<F,S>(first, second);
}

}

#endif /* STYLES_H_ */
