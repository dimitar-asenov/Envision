/***********************************************************************************************************************
 * VArrayInitializerStyle.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARRAYINITIALIZERSTYLE_H_
#define VARRAYINITIALIZERSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/VListStyle.h"
#include "VisualizationBase/headers/layouts/GridLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VArrayInitializerStyle : public Visualization::ItemStyle
{
	private:
		Visualization::GridLayoutStyle layout_;
		Visualization::VListStyle values_;

	public:
		void load();

		const Visualization::GridLayoutStyle& layout() const;
		const Visualization::VListStyle& values() const;

		static VArrayInitializerStyle* getDefault();
};

inline const Visualization::GridLayoutStyle& VArrayInitializerStyle::layout() const { return layout_; }
inline const Visualization::VListStyle& VArrayInitializerStyle::values() const { return values_; }

}

#endif /* VARRAYINITIALIZERSTYLE_H_ */
