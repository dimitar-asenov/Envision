/***********************************************************************************************************************
 * ShapeStyle.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SHAPESTYLE_H_
#define SHAPESTYLE_H_

#include "visualizationbase_api.h"

#include <QtGui/QPen>

namespace Visualization {

class VISUALIZATIONBASE_API ShapeStyle
{
	protected:
		QPen outline_;

	public:
		ShapeStyle();

		const QPen& outline();

		virtual ~ShapeStyle();
};

inline const QPen& ShapeStyle::outline() { return outline_; }
}

#endif /* SHAPESTYLE_H_ */
