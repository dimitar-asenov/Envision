/***********************************************************************************************************************
 * Attribute.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Extendable/Attribute.h"

namespace Model {

Attribute::Attribute()
{

}

Attribute::Attribute(QString name, QString type, bool optional, bool partialHint, bool persistent) :
	name_(name), type_(type), optional_(optional), partialHint_(partialHint), persistent_(persistent)
{
}

Attribute::Attribute(QString name) :
	name_(name)
{
}

const QString& Attribute::name() const
{
	return name_;
}

const QString& Attribute::type() const
{
	return type_;
}

bool Attribute::optional() const
{
	return optional_;
}

bool Attribute::partialHint() const
{
	return partialHint_;
}

bool Attribute::persistent() const
{
	return persistent_;
}

bool Attribute::operator==(const Attribute &other) const
{
	return name_ == other.name_;
}

}
