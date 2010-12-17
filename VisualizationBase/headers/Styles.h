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

#include <QtXml/QDomDocument>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QMap>
#include <QtCore/QDir>
#include <QtCore/QList>

class QColor;
class QPen;
class QFont;

namespace Visualization {

class VISUALIZATIONBASE_API Prototype
{
	public:
		QDomDocument doc;
		QDomElement elem;
};

class VISUALIZATIONBASE_API Styles
{
	private:
		static const QString STYLES_DIRECTORY_NAME;
		static const QString XML_DOM_TYPE;

		static QMutex accessMutex;
		static QDomElement elem;
		static QString currentPath;
		static QList<Prototype>* prototypes;

		static QDomDocument openStyleDoc(const QString& path);
		static void openNode(QDomNode node, const QString& property, QList<Prototype>& prototypes);
		static void loadElementPrototypes();
		static QDomElement getProperty(QDomNode& root, const QString& name);

		template <class T> static QMap<QString, typename T::StyleType*>& itemStyles();
		template <class T> static QMap<QString, typename T::StyleType*>& layoutStyles();
		template <class T> static QMap<QString, typename T::StyleType*>& shapeStyles();

		template <class T> static typename T::StyleType* loadStyle(const QString& objectClass, const QString& styleName);

		static void loadQColor(QColor& value);
		static void loadQPen(QPen& value);
		static void loadQFont(QFont& value);

	public:

		static void load(const QString& propertyName, int& value);
		static void load(const QString& propertyName, bool& value);
		static void load(const QString& propertyName, QString& value);
		static void load(const QString& propertyName, double& value);
		template <class T> static void load(const QString& propertyName, T& value);
		template <class T> static void load(T& value);

		template <class T> static typename T::StyleType* item(const QString& styleName = QString());
		template <class T> static typename T::StyleType* layout(const QString& styleName = QString());
		template <class T> static typename T::StyleType* shape(const QString& styleName = QString());
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

template<class T> typename T::StyleType* Styles::loadStyle(const QString& objectClass, const QString& styleName)
{
	QMutexLocker locker(&accessMutex);

	currentPath = QDir::current().absoluteFilePath(STYLES_DIRECTORY_NAME + "/" + objectClass + "/" + T::className());
	QDomDocument doc = openStyleDoc(currentPath + "/" + styleName);
	if (doc.isNull()) return NULL;

	QList<Prototype> proto;
	openNode(elem, "style", proto);

	typename T::StyleType* style = new typename T::StyleType();
	style->load();
	return style;
}

template <class T> void Styles::load(const QString& propertyName, T& value)
{
	QDomElement oldElem = elem;
	QList<Prototype>* oldPrototypes = prototypes;

	QList<Prototype> proto;
	openNode(elem, propertyName, proto);

	load(value);

	elem = oldElem;
	prototypes = oldPrototypes;
}

template <class T> inline void Styles::load(T& value) { value.load(); }
template <> inline void Styles::load<QPen>(QPen& value) { loadQPen(value); }
template <> inline void Styles::load<QColor>(QColor& value) { loadQColor(value); }
template <> inline void Styles::load<QFont>(QFont& value) { loadQFont(value); }

}

#endif /* STYLES_H_ */
