/***********************************************************************************************************************
 * VFormalResultStyle.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFORMALRESULTSTYLE_H_
#define VFORMALRESULTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VFormalResultStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle name_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& name() const;
};

inline const Visualization::SequentialLayoutStyle& VFormalResultStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VFormalResultStyle::name() const { return name_; }

}

#endif /* VFORMALRESULTSTYLE_H_ */
