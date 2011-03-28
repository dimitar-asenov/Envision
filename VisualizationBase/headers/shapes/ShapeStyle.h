/***********************************************************************************************************************
 * ShapeStyle.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SHAPESTYLE_H_
#define SHAPESTYLE_H_

#include "../visualizationbase_api.h"
#include "../styles/Style.h"

#include <QtGui/QPen>

namespace Visualization {

class VISUALIZATIONBASE_API ShapeStyle : public Style
{
	protected:
		QPen outline_;

	public:
		ShapeStyle();
		virtual void load(StyleLoader& sl);

		void setOutline(const QPen& outline);
		const QPen& outline() const;

		virtual ~ShapeStyle();
};

inline const QPen& ShapeStyle::outline() const { return outline_; }
}

#endif /* SHAPESTYLE_H_ */
