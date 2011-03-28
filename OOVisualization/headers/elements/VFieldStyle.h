/***********************************************************************************************************************
 * VFieldStyle.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFIELDSTYLE_H_
#define VFIELDSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VFieldStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle nameDefault_;
		Visualization::TextStyle namePublic_;
		Visualization::TextStyle namePrivate_;
		Visualization::TextStyle nameProtected_;
		Visualization::TextStyle nameStaticDefault_;
		Visualization::TextStyle nameStaticPublic_;
		Visualization::TextStyle nameStaticPrivate_;
		Visualization::TextStyle nameStaticProtected_;

	public:
		VFieldStyle();
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& nameDefault() const;
		const Visualization::TextStyle& namePublic() const;
		const Visualization::TextStyle& namePrivate() const;
		const Visualization::TextStyle& nameProtected() const;
		const Visualization::TextStyle& nameStaticDefault() const;
		const Visualization::TextStyle& nameStaticPublic() const;
		const Visualization::TextStyle& nameStaticPrivate() const;
		const Visualization::TextStyle& nameStaticProtected() const;
};

inline const Visualization::SequentialLayoutStyle& VFieldStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VFieldStyle::nameDefault() const { return nameDefault_; }
inline const Visualization::TextStyle& VFieldStyle::namePublic() const { return namePublic_; }
inline const Visualization::TextStyle& VFieldStyle::namePrivate() const { return namePrivate_; }
inline const Visualization::TextStyle& VFieldStyle::nameProtected() const { return nameProtected_; }
inline const Visualization::TextStyle& VFieldStyle::nameStaticDefault() const { return nameStaticDefault_; }
inline const Visualization::TextStyle& VFieldStyle::nameStaticPublic() const { return nameStaticPublic_; }
inline const Visualization::TextStyle& VFieldStyle::nameStaticPrivate() const { return nameStaticPrivate_; }
inline const Visualization::TextStyle& VFieldStyle::nameStaticProtected() const { return nameStaticProtected_; }

}

#endif /* VFIELDSTYLE_H_ */
