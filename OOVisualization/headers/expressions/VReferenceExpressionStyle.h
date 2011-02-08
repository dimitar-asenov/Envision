/***********************************************************************************************************************
 * VReferenceExpressionStyle.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VREFERENCEEXPRESSIONSTYLE_H_
#define VREFERENCEEXPRESSIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VReferenceExpressionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::TextStyle name_;
		Visualization::TextStyle separator_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::TextStyle& name() const;
		const Visualization::TextStyle& separator() const;

		static VReferenceExpressionStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VReferenceExpressionStyle::container() const { return container_; }
inline const Visualization::TextStyle& VReferenceExpressionStyle::name() const { return name_; }
inline const Visualization::TextStyle& VReferenceExpressionStyle::separator() const { return separator_; }

}

#endif /* VREFERENCEEXPRESSIONSTYLE_H_ */
