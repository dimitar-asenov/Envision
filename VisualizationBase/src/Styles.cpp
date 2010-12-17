/***********************************************************************************************************************
 * Styles.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Styles.h"

#include <QtGui/QPen>
#include <QtGui/QFont>
#include <QtGui/QColor>

namespace Visualization {

const QString Styles::STYLES_DIRECTORY_NAME = "styles";
const QString Styles::XML_DOM_TYPE = "EnvisionVisualizationStyle";

QMutex Styles::accessMutex;
QDomElement Styles::elem;
QString Styles::currentPath;
QList<Prototype>* Styles::prototypes = NULL;

QDomDocument Styles::openStyleDoc(const QString& path)
{
	QDomDocument doc = QDomDocument(XML_DOM_TYPE);
	QFile file(path);
	if ( !file.open(QIODevice::ReadOnly) ) return QDomDocument();
	if ( !doc.setContent(&file) )
	{
		file.close();
		throw VisualizationException("Reading of the XML structure of file " + file.fileName() + " failed.");
	}
	file.close();
	return doc;
}

void Styles::openNode(QDomNode node, const QString& property, QList<Prototype>& prototypes_)
{
	elem = getProperty(node, property);
	prototypes = &prototypes_;
	loadElementPrototypes();
}

void Styles::loadElementPrototypes()
{
	QDomElement e = elem;
	QString path = currentPath;
	QString newPath;

	while ( e.hasAttribute("prototype") )
	{
		QString prototypePath = e.attribute("prototype");
		prototypes->append(Prototype());

		// Try to use a relative location.
		newPath = path + "/" + prototypePath;
		prototypes->last().doc = openStyleDoc(newPath);

		// Try to use a location starting from the root of the styles folder
		if ( prototypes->last().doc.isNull() )
		{
			newPath = QDir::current().absoluteFilePath(STYLES_DIRECTORY_NAME + "/" + prototypePath);
			prototypes->last().doc = openStyleDoc(newPath);
		}

		// If prototype is not found, fail
		if ( prototypes->last().doc.isNull() ) throw VisualizationException("Could not find the prototype style " + prototypePath);

		prototypes->last().elem = prototypes->last().doc.firstChildElement("style");
		if ( prototypes->last().elem.isNull() ) throw VisualizationException("Invalid prototype file '" + prototypePath
				+ "'. The file does not define a style.");

		path = newPath;
		e = prototypes->last().elem;
	}
}

QDomElement Styles::getProperty(QDomNode& root, const QString& name)
{
	QDomElement elem = root.firstChildElement(name);

	int index = 0;
	while (elem.isNull() && prototypes && index < prototypes->size())
	{
		elem = prototypes->at(index).elem.firstChildElement(name);
		++index;
	}

	if (!elem.isNull()) return elem;
	else throw VisualizationException("Invalid style file, property '" + name + "' is missing.");
}

void Styles::load(const QString& propertyName, int& value)
{
	bool ok = true;

	value = getProperty(elem,propertyName).firstChild().nodeValue().toInt(&ok);
	if ( !ok ) throw VisualizationException("Could read integer value property '" + propertyName + "'");
}

void Styles::load(const QString& propertyName, bool& value)
{
	value = getProperty(elem,propertyName).firstChild().nodeValue().compare("true", Qt::CaseInsensitive) == 0;
}

void Styles::load(const QString& propertyName, QString& value)
{
	value = getProperty(elem,propertyName).firstChild().nodeValue();
}

void Styles::load(const QString& propertyName, double& value)
{
	bool ok = true;

	value = getProperty(elem,propertyName).firstChild().nodeValue().toDouble(&ok);
	if ( !ok ) throw VisualizationException("Could read double value property '" + propertyName + "'");
}

void Styles::loadQColor(QColor& value)
{
	int a, r, g, b;
	Styles::load("alpha", a);
	Styles::load("red", r);
	Styles::load("green", g);
	Styles::load("blue", b);

	value = QColor(r, g, b, a);
}

void Styles::loadQPen(QPen& value)
{
	double width;
	int style, capStyle, joinStyle;
	QColor color;

	Styles::load("width", width);
	Styles::load("style", style);
	Styles::load("color", color);
	Styles::load("capStyle", capStyle);
	Styles::load("joinStyle", joinStyle);
	value = QPen(QBrush(color), width, (Qt::PenStyle) style, (Qt::PenCapStyle) capStyle, (Qt::PenJoinStyle) joinStyle);
}

void Styles::loadQFont(QFont& value)
{
	QString family;
	int size;
	bool pixelSize;
	int weight;
	int style;

	Styles::load("family", family);
	Styles::load("size", size);
	Styles::load("sizeIsInPixels", pixelSize);
	Styles::load("weight", weight);
	Styles::load("style", style);

	value = QFont();
	if ( !family.isEmpty() ) value.setFamily(family);
	value.setWeight(weight);
	if ( pixelSize ) value.setPixelSize(size);
	else value.setPointSize(size);
	value.setStyle((QFont::Style) style);
}

}
