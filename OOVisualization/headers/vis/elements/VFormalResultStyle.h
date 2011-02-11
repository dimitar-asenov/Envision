/***********************************************************************************************************************
 * VFormalResultStyle.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFORMALRESULTSTYLE_H_
#define VFORMALRESULTSTYLE_H_

#include "../../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VFormalResultStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::TextStyle name_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::TextStyle& name() const;

		static VFormalResultStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VFormalResultStyle::container() const { return container_; }
inline const Visualization::TextStyle& VFormalResultStyle::name() const { return name_; }

}

#endif /* VFORMALRESULTSTYLE_H_ */
