/***********************************************************************************************************************
 * DiamondStyle.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef DIAMONDSTYLE_H_
#define DIAMONDSTYLE_H_

#include "../visualizationbase_api.h"

#include "ShapeStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API DiamondStyle : public ShapeStyle
{
	protected:
		QBrush background_;

		QBrush shadow_;
		int xShadowOffset_;
		int yShadowOffset_;

	public:
		DiamondStyle();
		virtual void load();

		const QBrush& background() const;


		const QBrush& shadow() const;
		int xShadowOffset() const;
		int yShadowOffset() const;

		static DiamondStyle* getDefault();
};

inline const QBrush& DiamondStyle::background() const { return background_; };
inline const QBrush& DiamondStyle::shadow() const { return shadow_; };
inline int DiamondStyle::xShadowOffset() const { return xShadowOffset_; };
inline int DiamondStyle::yShadowOffset() const { return yShadowOffset_; };

}

#endif /* DIAMONDSTYLE_H_ */
