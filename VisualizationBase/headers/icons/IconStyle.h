/***********************************************************************************************************************
 * IconStyle.h
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ICONSTYLE_H_
#define ICONSTYLE_H_

#include "../visualizationbase_api.h"
#include "../items/ItemStyle.h"

#include <QtGui/QPen>

namespace Visualization {

class VISUALIZATIONBASE_API IconStyle : public ItemStyle
{
	private:
		QPen outline_;

	public:
		virtual void load(StyleLoader& sl);

		const QPen& outline() const;
};

inline const QPen& IconStyle::outline() const { return outline_; }

}

#endif /* ICONSTYLE_H_ */
