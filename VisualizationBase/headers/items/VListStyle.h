/***********************************************************************************************************************
 * VListStyle.h
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLISTSTYLE_H_
#define VLISTSTYLE_H_

#include "../visualizationbase_api.h"

#include "../layouts/SequentialLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API VListStyle : public ItemStyle
{
	private:
		SequentialLayoutStyle itemsStyle_;
		bool drawShapeWhenEmpty_;

	public:
		VListStyle();
		virtual void load();

		const SequentialLayoutStyle&  itemsStyle() const;
		bool drawShapeWhenEmpty() const;

		static VListStyle* getDefault();
};

inline const SequentialLayoutStyle& VListStyle::itemsStyle() const {return itemsStyle_; }
inline bool VListStyle::drawShapeWhenEmpty() const {return drawShapeWhenEmpty_; }

}

#endif /* VLISTSTYLE_H_ */
