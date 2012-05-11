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
 * BoxStyle.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOXSTYLE_H_
#define BOXSTYLE_H_

#include "../visualizationbase_api.h"

#include "ShapeStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API BoxStyle : public ShapeStyle
{
	public:
		typedef enum {RightAngleCorner, RoundCorner, CutCorner} CornerType;

	protected:
		QBrush background_;

		CornerType corner_;
		int cornerRadius_;

		QBrush shadow_;
		int xShadowOffset_;
		int yShadowOffset_;

	public:
		virtual void load(StyleLoader& sl);

		const QBrush& background() const;

		CornerType corner() const;
		int cornerRadius() const;

		const QBrush& shadow() const;
		int xShadowOffset() const;
		int yShadowOffset() const;
};

inline const QBrush& BoxStyle::background() const { return background_; }
inline BoxStyle::CornerType BoxStyle::corner() const { return corner_; }
inline int BoxStyle::cornerRadius() const { return cornerRadius_; }
inline const QBrush& BoxStyle::shadow() const { return shadow_; }
inline int BoxStyle::xShadowOffset() const { return xShadowOffset_; }
inline int BoxStyle::yShadowOffset() const { return yShadowOffset_; }

}

#endif /* BOXSTYLE_H_ */
