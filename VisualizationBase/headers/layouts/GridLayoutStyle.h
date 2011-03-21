/***********************************************************************************************************************
 * GridLayoutStyle.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef GRIDLAYOUTSTYLE_H_
#define GRIDLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API GridLayoutStyle : public LayoutStyle
{
	private:
		Alignment horizontalAlignment_;
		Alignment verticalAlignment_;
		int horizontalSpace_;
		int verticalSpace_;

	public:
		virtual void load();

		Alignment horizontalAlignment() const;
		Alignment verticalAlignment() const;
		int horizontalSpace() const;
		int verticalSpace() const;

		static GridLayoutStyle* getDefault();
};

inline LayoutStyle::Alignment GridLayoutStyle::horizontalAlignment() const { return horizontalAlignment_; }
inline LayoutStyle::Alignment GridLayoutStyle::verticalAlignment() const { return verticalAlignment_; }
inline int GridLayoutStyle::horizontalSpace() const { return horizontalSpace_; }
inline int GridLayoutStyle::verticalSpace() const { return verticalSpace_; }

}

#endif /* GRIDLAYOUTSTYLE_H_ */
