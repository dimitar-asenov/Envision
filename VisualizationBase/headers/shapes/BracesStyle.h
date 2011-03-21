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
		QPen leftBracePen_;
		QFont leftBraceFont_;
		QString leftBrace_;

		QPen rightBracePen_;
		QFont rightBraceFont_;
		QString rightBrace_;

	public:
		virtual void load();

		const QPen& leftBracePen() const;
		const QFont& leftBraceFont() const;
		const QString& leftBrace() const;

		const QPen& rightBracePen() const;
		const QFont& rightBraceFont() const;
		const QString& rightBrace() const;

		static BracesStyle* getDefault();
};

inline const QPen& BracesStyle::leftBracePen() const { return leftBracePen_; }
inline const QFont& BracesStyle::leftBraceFont() const { return leftBraceFont_; }
inline const QString& BracesStyle::leftBrace() const { return leftBrace_; }

inline const QPen& BracesStyle::rightBracePen() const { return rightBracePen_; }
inline const QFont& BracesStyle::rightBraceFont() const { return rightBraceFont_; }
inline const QString& BracesStyle::rightBrace() const { return rightBrace_; }

}

#endif /* BRACESSTYLE_H_ */
