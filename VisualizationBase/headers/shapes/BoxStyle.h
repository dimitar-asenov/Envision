/***********************************************************************************************************************
 * BoxStyle.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOXSTYLE_H_
#define BOXSTYLE_H_

#include "visualizationbase_api.h"

#include "ShapeStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API BoxStyle : public ShapeStyle
{
	public:
		typedef enum {RightAngleCorner, RoundCorner, CutCorner} CornerType;

	protected:
		QBrush background_;

		CornerType corner_;
		int cornerRadius_;

		QBrush shadow_;
		int xShadowOffset_;
		int yShadowOffset_;

	public:
		BoxStyle();

		void setBackground(const QBrush& brush);

		void setCorner(CornerType type, int radius);
		void setShadow(const QBrush& brush, int xOffset, int yOffset);

		const QBrush& background() const;

		CornerType corner() const;
		int cornerRadius() const;

		const QBrush& shadow() const;
		int xShadowOffset() const;
		int yShadowOffset() const;


		static BoxStyle* getDefault();
};

inline const QBrush& BoxStyle::background() const { return background_; };
inline BoxStyle::CornerType BoxStyle::corner() const { return corner_; };
inline int BoxStyle::cornerRadius() const { return cornerRadius_; };
inline const QBrush& BoxStyle::shadow() const { return shadow_; };
inline int BoxStyle::xShadowOffset() const { return xShadowOffset_; };
inline int BoxStyle::yShadowOffset() const { return yShadowOffset_; };

}

#endif /* BOXSTYLE_H_ */
