/***********************************************************************************************************************
 * PositionLayoutStyle.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POSITIONLAYOUTSTYLE_H_
#define POSITIONLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PositionLayoutStyle : public LayoutStyle
{
	private:
		int spaceBetweenElements_;

	public:
		PositionLayoutStyle();
		virtual void load();

		int spaceBetweenElements() const;

		void setSpaceBetweenElements(int space);

		static PositionLayoutStyle* getDefault();
};

inline int PositionLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }

}

#endif /* POSITIONLAYOUTSTYLE_H_ */
