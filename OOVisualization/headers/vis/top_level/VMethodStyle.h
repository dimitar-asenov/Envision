/***********************************************************************************************************************
 * VMethodStyle.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODSTYLE_H_
#define VMETHODSTYLE_H_

#include "../../oovisualization_api.h"
#include "icons/MethodIconStyle.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/VListStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VMethodStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle border_;
		Visualization::TextStyle nameDefault_;
		Visualization::TextStyle namePublic_;
		Visualization::TextStyle namePrivate_;
		Visualization::TextStyle nameProtected_;
		Visualization::TextStyle nameStaticDefault_;
		Visualization::TextStyle nameStaticPublic_;
		Visualization::TextStyle nameStaticPrivate_;
		Visualization::TextStyle nameStaticProtected_;
		MethodIconStyle icon_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::VListStyle content_;
		Visualization::VListStyle arguments_;
		Visualization::VListStyle results_;

	public:
		VMethodStyle();
		void load();

		const Visualization::PanelBorderLayoutStyle& border() const;
		const Visualization::TextStyle& nameDefault() const;
		const Visualization::TextStyle& namePublic() const;
		const Visualization::TextStyle& namePrivate() const;
		const Visualization::TextStyle& nameProtected() const;
		const Visualization::TextStyle& nameStaticDefault() const;
		const Visualization::TextStyle& nameStaticPublic() const;
		const Visualization::TextStyle& nameStaticPrivate() const;
		const Visualization::TextStyle& nameStaticProtected() const;
		const MethodIconStyle& icon() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::VListStyle& content() const;
		const Visualization::VListStyle& arguments() const;
		const Visualization::VListStyle& results() const;

		static VMethodStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VMethodStyle::border() const { return border_; }
inline const Visualization::TextStyle& VMethodStyle::nameDefault() const { return nameDefault_; }
inline const Visualization::TextStyle& VMethodStyle::namePublic() const { return namePublic_; }
inline const Visualization::TextStyle& VMethodStyle::namePrivate() const { return namePrivate_; }
inline const Visualization::TextStyle& VMethodStyle::nameProtected() const { return nameProtected_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticDefault() const { return nameStaticDefault_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticPublic() const { return nameStaticPublic_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticPrivate() const { return nameStaticPrivate_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticProtected() const { return nameStaticProtected_; }
inline const MethodIconStyle& VMethodStyle::icon() const { return icon_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::header() const { return header_; }
inline const Visualization::VListStyle& VMethodStyle::content() const { return content_; }
inline const Visualization::VListStyle& VMethodStyle::arguments() const { return arguments_; }
inline const Visualization::VListStyle& VMethodStyle::results() const { return results_; }

}

#endif /* VMETHODSTYLE_H_ */
