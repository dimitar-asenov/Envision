/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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
		virtual void load(StyleLoader& sl);

		int leftInnerMargin() const;
		int rightInnerMargin() const;
		int topInnerMargin() const;
		int bottomInnerMargin() const;
		int spaceBetweenElements() const;
		int gridSize() const;
};

inline int PositionLayoutStyle::leftInnerMargin() const { return leftInnerMargin_; }
inline int PositionLayoutStyle::rightInnerMargin() const { return rightInnerMargin_; }
inline int PositionLayoutStyle::topInnerMargin() const { return topInnerMargin_; }
inline int PositionLayoutStyle::bottomInnerMargin() const {return bottomInnerMargin_; }
inline int PositionLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }
inline int PositionLayoutStyle::gridSize() const {	return gridSize_; }

}

#endif /* POSITIONLAYOUTSTYLE_H_ */
