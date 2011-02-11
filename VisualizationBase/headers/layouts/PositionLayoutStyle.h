/***********************************************************************************************************************
 * PositionLayoutStyle.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POSITIONLAYOUTSTYLE_H_
#define POSITIONLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PositionLayoutStyle : public LayoutStyle
{
	private:
		int leftInnerMargin_;
		int rightInnerMargin_;
		int topInnerMargin_;
		int bottomInnerMargin_;
		int spaceBetweenElements_;
		int gridSize_;

	public:
		PositionLayoutStyle();
		virtual void load();

		int leftInnerMargin() const;
		int rightInnerMargin() const;
		int topInnerMargin() const;
		int bottomInnerMargin() const;
		int spaceBetweenElements() const;
		int gridSize() const;

		static PositionLayoutStyle* getDefault();
};

inline int PositionLayoutStyle::leftInnerMargin() const { return leftInnerMargin_; }
inline int PositionLayoutStyle::rightInnerMargin() const { return rightInnerMargin_; }
inline int PositionLayoutStyle::topInnerMargin() const { return topInnerMargin_; }
inline int PositionLayoutStyle::bottomInnerMargin() const {return bottomInnerMargin_; }
inline int PositionLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }
inline int PositionLayoutStyle::gridSize() const {	return gridSize_; }

}

#endif /* POSITIONLAYOUTSTYLE_H_ */
