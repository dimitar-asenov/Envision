/***********************************************************************************************************************
 * VMethodStyle.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODSTYLE_H_
#define VMETHODSTYLE_H_

#include "../../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VMethodStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle border_;
		Visualization::TextStyle name_;
		Visualization::TextStyle nameStatic_;
		Visualization::TextStyle visibility_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::SequentialLayoutStyle nameContainer_;
		Visualization::SequentialLayoutStyle content_;
		Visualization::SequentialLayoutStyle arguments_;
		Visualization::SequentialLayoutStyle results_;

	public:
		VMethodStyle();
		void load();

		const Visualization::PanelBorderLayoutStyle& border() const;
		const Visualization::TextStyle& name() const;
		const Visualization::TextStyle& nameStatic() const;
		const Visualization::TextStyle& visibility() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::SequentialLayoutStyle& nameContainer() const;
		const Visualization::SequentialLayoutStyle& content() const;
		const Visualization::SequentialLayoutStyle& arguments() const;
		const Visualization::SequentialLayoutStyle& results() const;

		static VMethodStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VMethodStyle::border() const { return border_; }
inline const Visualization::TextStyle& VMethodStyle::name() const { return name_; }
inline const Visualization::TextStyle& VMethodStyle::nameStatic() const { return nameStatic_; }
inline const Visualization::TextStyle& VMethodStyle::visibility() const { return visibility_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::header() const { return header_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::nameContainer() const { return nameContainer_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::content() const { return content_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::arguments() const { return arguments_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::results() const { return results_; }

}

#endif /* VMETHODSTYLE_H_ */
