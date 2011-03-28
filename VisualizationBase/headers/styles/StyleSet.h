/***********************************************************************************************************************
 * StyleSet.h
 *
 *  Created on: Mar 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STYLESET_H_
#define STYLESET_H_

#include "../VisualizationException.h"
#include "StyleLoader.h"

namespace Visualization {

template<class T> class StyleSet
{
	public:
		StyleSet(const QString& classType);
		~StyleSet();

		typename T::StyleType* get(const QString& styleName = QString());

	private:
		QMap<QString, typename T::StyleType*> styles_;

		/*
		 * This determines the folder inside the root 'styles' folder that will be used to look for this item's own
		 * directory. Typical values are "item", "shape", "layout" and "icon".
		 */
		QString classType_;
};

template<class T> StyleSet<T>::StyleSet(const QString& classType) : classType_(classType) {}
template<class T> StyleSet<T>::~StyleSet() {}


template<class T> typename T::StyleType* StyleSet<T>::get(const QString& styleName)
{
	QString name(styleName);
	if (name.isEmpty()) name = "default";

	typename T::StyleType* style = styles_.value(name, NULL);
	if (style) return style;

	style = StyleLoader().loadStyle<typename T::StyleType>(classType_ + "/" + T::className(), name);
	if (style)
	{
		styles_.insert(name,style);
		return style;
	}
	else throw VisualizationException("Could not find the style '" + classType_ + "/" + T::className() + "/" + styleName + "'");
}

}

#endif /* STYLESET_H_ */
