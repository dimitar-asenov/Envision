/***********************************************************************************************************************
 * VClassStyle.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCLASSSTYLE_H_
#define VCLASSSTYLE_H_

#include "../oovisualization_api.h"

#include "icons/ClassIconStyle.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VClassStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle border_;
		Visualization::TextStyle nameDefault_;
		Visualization::TextStyle namePublic_;
		Visualization::TextStyle namePrivate_;
		Visualization::TextStyle nameProtected_;
		ClassIconStyle icon_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::PositionLayoutStyle content_;
		Visualization::SequentialLayoutStyle fieldContainer_;
		Visualization::SequentialLayoutStyle publicFieldArea_;
		Visualization::SequentialLayoutStyle privateFieldArea_;
		Visualization::SequentialLayoutStyle protectedFieldArea_;
		Visualization::SequentialLayoutStyle defaultFieldArea_;

	public:
		VClassStyle();
		void load();

		const Visualization::PanelBorderLayoutStyle& border() const;
		const Visualization::TextStyle& nameDefault() const;
		const Visualization::TextStyle& namePublic() const;
		const Visualization::TextStyle& namePrivate() const;
		const Visualization::TextStyle& nameProtected() const;
		const ClassIconStyle& icon() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::PositionLayoutStyle& content() const;
		const Visualization::SequentialLayoutStyle& fieldContainer() const;
		const Visualization::SequentialLayoutStyle& publicFieldArea() const;
		const Visualization::SequentialLayoutStyle& privateFieldArea() const;
		const Visualization::SequentialLayoutStyle& protectedFieldArea() const;
		const Visualization::SequentialLayoutStyle& defaultFieldArea() const;

		static VClassStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VClassStyle::border() const { return border_; }
inline const Visualization::TextStyle& VClassStyle::nameDefault() const { return nameDefault_; }
inline const Visualization::TextStyle& VClassStyle::namePublic() const { return namePublic_; }
inline const Visualization::TextStyle& VClassStyle::namePrivate() const { return namePrivate_; }
inline const Visualization::TextStyle& VClassStyle::nameProtected() const { return nameProtected_; }
inline const ClassIconStyle& VClassStyle::icon() const { return icon_; }
inline const Visualization::SequentialLayoutStyle& VClassStyle::header() const { return header_; }
inline const Visualization::PositionLayoutStyle& VClassStyle::content() const { return content_; }
inline const Visualization::SequentialLayoutStyle& VClassStyle::fieldContainer() const { return fieldContainer_; }
inline const Visualization::SequentialLayoutStyle& VClassStyle::publicFieldArea() const { return publicFieldArea_; }
inline const Visualization::SequentialLayoutStyle& VClassStyle::privateFieldArea() const { return privateFieldArea_; }
inline const Visualization::SequentialLayoutStyle& VClassStyle::protectedFieldArea() const { return protectedFieldArea_; }
inline const Visualization::SequentialLayoutStyle& VClassStyle::defaultFieldArea() const { return defaultFieldArea_; }

}

#endif /* VCLASSSTYLE_H_ */
