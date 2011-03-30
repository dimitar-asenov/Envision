/***********************************************************************************************************************
 * StaticStyle.h
 *
 *  Created on: Mar 30, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STATICSTYLE_H_
#define STATICSTYLE_H_

#include "../visualizationbase_api.h"

#include "ItemStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API StaticStyle : public ItemStyle
{
	private:
		QString itemClass_;
		ItemStyle* itemStyle_;

	public:
		StaticStyle();
		virtual ~StaticStyle();

		void load(StyleLoader& sl);

		const QString& itemClass() const;
		const ItemStyle& itemStyle() const;
};

inline const QString& StaticStyle::itemClass() const { return itemClass_; }
inline const ItemStyle& StaticStyle::itemStyle() const { return *itemStyle_; }

}

#endif /* STATICSTYLE_H_ */
