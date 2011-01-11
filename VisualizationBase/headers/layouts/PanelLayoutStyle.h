/***********************************************************************************************************************
 * PanelLayoutStyle.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELLAYOUTSTYLE_H_
#define PANELLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"
#include "SequentialLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelLayoutStyle : public LayoutStyle
{
	private:
		Orientation orientation_;
		Alignment alignment_;
		int spaceBetweenElements_;

	public:
		PanelLayoutStyle();
		virtual void load();

		Orientation orientation() const;
		Alignment alignment() const;
		int spaceBetweenElements() const;

		void setOrientation(Orientation orientation);
		void setAlignment(Alignment alignment);
		void setSpaceBetweenElements(int space);

		static PanelLayoutStyle* getDefault();
};

inline LayoutStyle::Orientation PanelLayoutStyle::orientation() const { return orientation_; }
inline LayoutStyle::Alignment PanelLayoutStyle::alignment() const {	return alignment_; }
inline int PanelLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }

}

#endif /* PANELLAYOUTSTYLE_H_ */
