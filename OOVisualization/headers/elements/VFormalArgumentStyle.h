/***********************************************************************************************************************
 * VFormalArgumentStyle.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFORMALARGUMENTSTYLE_H_
#define VFORMALARGUMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VFormalArgumentStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::TextStyle name_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::TextStyle& name() const;

		static VFormalArgumentStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VFormalArgumentStyle::container() const { return container_; }
inline const Visualization::TextStyle& VFormalArgumentStyle::name() const { return name_; }

}

#endif /* VFORMALARGUMENTSTYLE_H_ */
