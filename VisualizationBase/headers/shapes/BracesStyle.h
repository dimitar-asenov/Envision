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
 * BracesStyle.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BRACESSTYLE_H_
#define BRACESSTYLE_H_

#include "../visualizationbase_api.h"

#include "ShapeStyle.h"

#include <QtGui/QFont>

namespace Visualization {

class VISUALIZATIONBASE_API BracesStyle : public ShapeStyle
{
	protected:
		int minHeight_;
		QPen leftBracePen_;
		QFont leftBraceFont_;
		QString leftBrace_;

		QPen rightBracePen_;
		QFont rightBraceFont_;
		QString rightBrace_;

	public:
		virtual void load(StyleLoader& sl);

		int minHeight() const;
		const QPen& leftBracePen() const;
		const QFont& leftBraceFont() const;
		const QString& leftBrace() const;

		const QPen& rightBracePen() const;
		const QFont& rightBraceFont() const;
		const QString& rightBrace() const;
};

inline int BracesStyle::minHeight() const { return minHeight_; }
inline const QPen& BracesStyle::leftBracePen() const { return leftBracePen_; }
inline const QFont& BracesStyle::leftBraceFont() const { return leftBraceFont_; }
inline const QString& BracesStyle::leftBrace() const { return leftBrace_; }

inline const QPen& BracesStyle::rightBracePen() const { return rightBracePen_; }
inline const QFont& BracesStyle::rightBraceFont() const { return rightBraceFont_; }
inline const QString& BracesStyle::rightBrace() const { return rightBrace_; }

}

#endif /* BRACESSTYLE_H_ */
