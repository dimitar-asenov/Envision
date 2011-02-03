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
		int spaceBetweenElements_;
		int gridSize_;

	public:
		PositionLayoutStyle();
		virtual void load();

		int spaceBetweenElements() const;
		int gridSize() const;

		static PositionLayoutStyle* getDefault();
};

inline int PositionLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }
inline int PositionLayoutStyle::gridSize() const {	return gridSize_; }

}

#endif /* POSITIONLAYOUTSTYLE_H_ */
