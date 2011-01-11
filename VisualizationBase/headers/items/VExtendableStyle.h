/***********************************************************************************************************************
 * VExtendableStyle.h
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VEXTENDABLESTYLE_H_
#define VEXTENDABLESTYLE_H_

#include "../visualizationbase_api.h"

#include "../layouts/PanelBorderLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API VExtendableStyle : public ItemStyle
{
	private:
		PanelBorderLayoutStyle borderStyle_;
		SequentialLayoutStyle headerStyle_;
		SequentialLayoutStyle attributesStyle_;
		bool expanded_;

	public:
		VExtendableStyle();
		void load();

		const PanelBorderLayoutStyle& borderStyle() const;
		const SequentialLayoutStyle&  headerStyle() const;
		const SequentialLayoutStyle&  attributesStyle() const;
		bool expanded() const;

		static VExtendableStyle* getDefault();
};

inline const PanelBorderLayoutStyle& VExtendableStyle::borderStyle() const {return borderStyle_; }
inline const SequentialLayoutStyle& VExtendableStyle::headerStyle() const {return headerStyle_; }
inline const SequentialLayoutStyle& VExtendableStyle::attributesStyle() const {return attributesStyle_; }
inline bool VExtendableStyle::expanded() const { return expanded_; };

}

#endif /* VEXTENDABLESTYLE_H_ */
