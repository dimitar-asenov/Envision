/***********************************************************************************************************************
 * PanelLayout.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELLAYOUT_H_
#define PANELLAYOUT_H_

#include "visualizationbase_api.h"

#include "Layout.h"
#include "PanelLayoutStyle.h"
#include "SequentialLayout.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelLayout: public Layout
{
	private:
		PanelLayoutStyle* style;

		SequentialLayout begin_;
		SequentialLayout center_;
		SequentialLayout end_;

	public:
		PanelLayout(Item* parent, PanelLayoutStyle* style = PanelLayoutStyle::getDefault());
		void setStyle(PanelLayoutStyle* style = PanelLayoutStyle::getDefault());

		SequentialLayout* begin();
		SequentialLayout* center();
		SequentialLayout* end();

		virtual void updateState();
};

inline SequentialLayout* PanelLayout::begin() { return &begin_; };
inline SequentialLayout* PanelLayout::center() { return &center_; };
inline SequentialLayout* PanelLayout::end() { return &end_; };

}

#endif /* PANELLAYOUT_H_ */
