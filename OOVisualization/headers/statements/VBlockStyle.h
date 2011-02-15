/***********************************************************************************************************************
 * VBlockStyle.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBLOCKSTYLE_H_
#define VBLOCKSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VListStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VBlockStyle : public Visualization::ItemStyle
{
	private:
		Visualization::VListStyle items_;

	public:
		void load();

		const Visualization::VListStyle& items() const;

		static VBlockStyle* getDefault();
};

inline const Visualization::VListStyle& VBlockStyle::items() const { return items_; }

}

#endif /* VBLOCKSTYLE_H_ */
