/***********************************************************************************************************************
 * VVariableAccessStyle.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVARIABLEACCESSSTYLE_H_
#define VVARIABLEACCESSSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VVariableAccessStyle : public Visualization::ItemStyle
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

		static VVariableAccessStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VVariableAccessStyle::container() const { return container_; }
inline const Visualization::TextStyle& VVariableAccessStyle::name() const { return name_; }
inline const Visualization::TextStyle& VVariableAccessStyle::separator() const { return separator_; }

}

#endif /* VVARIABLEACCESSSTYLE_H_ */
