/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#pragma once

#include "../oovisualization_api.h"


#include "VisualizationBase/src/items/TextStyle.h"
#include "VisualizationBase/src/items/VListStyle.h"
#include "VisualizationBase/src/items/StaticStyle.h"
#include "VisualizationBase/src/items/LineStyle.h"
#include "VisualizationBase/src/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/src/layouts/SequentialLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VMethodStyle : public Visualization::ItemStyle
{
	private:
		Visualization::TextStyle nameDefault_;
		Visualization::TextStyle namePublic_;
		Visualization::TextStyle namePrivate_;
		Visualization::TextStyle nameProtected_;
		Visualization::TextStyle nameStaticDefault_;
		Visualization::TextStyle nameStaticPublic_;
		Visualization::TextStyle nameStaticPrivate_;
		Visualization::TextStyle nameStaticProtected_;
		Visualization::StaticStyle icon_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::LineStyle signatureLine_;
		Visualization::VListStyle body_;
		Visualization::VListStyle annotations_;
		Visualization::SequentialLayoutStyle addons_;
		Visualization::SequentialLayoutStyle content_;
		Visualization::VListStyle typeArguments_;
		Visualization::VListStyle arguments_;
		Visualization::VListStyle results_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::TextStyle& nameDefault() const;
		const Visualization::TextStyle& namePublic() const;
		const Visualization::TextStyle& namePrivate() const;
		const Visualization::TextStyle& nameProtected() const;
		const Visualization::TextStyle& nameStaticDefault() const;
		const Visualization::TextStyle& nameStaticPublic() const;
		const Visualization::TextStyle& nameStaticPrivate() const;
		const Visualization::TextStyle& nameStaticProtected() const;
		const Visualization::StaticStyle& icon() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::LineStyle& signatureLine() const;
		const Visualization::VListStyle& body() const;
		const Visualization::VListStyle& annotations() const;
		const Visualization::SequentialLayoutStyle& addons() const;
		const Visualization::SequentialLayoutStyle& content() const;
		const Visualization::VListStyle& typeArguments() const;
		const Visualization::VListStyle& arguments() const;
		const Visualization::VListStyle& results() const;
};

inline const Visualization::TextStyle& VMethodStyle::nameDefault() const { return nameDefault_; }
inline const Visualization::TextStyle& VMethodStyle::namePublic() const { return namePublic_; }
inline const Visualization::TextStyle& VMethodStyle::namePrivate() const { return namePrivate_; }
inline const Visualization::TextStyle& VMethodStyle::nameProtected() const { return nameProtected_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticDefault() const { return nameStaticDefault_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticPublic() const { return nameStaticPublic_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticPrivate() const { return nameStaticPrivate_; }
inline const Visualization::TextStyle& VMethodStyle::nameStaticProtected() const { return nameStaticProtected_; }
inline const Visualization::StaticStyle& VMethodStyle::icon() const { return icon_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::header() const { return header_; }
inline const Visualization::LineStyle& VMethodStyle::signatureLine() const { return signatureLine_; }
inline const Visualization::VListStyle& VMethodStyle::body() const { return body_; }
inline const Visualization::VListStyle& VMethodStyle::annotations() const { return annotations_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::addons() const { return addons_; }
inline const Visualization::SequentialLayoutStyle& VMethodStyle::content() const { return content_; }
inline const Visualization::VListStyle& VMethodStyle::arguments() const { return arguments_; }
inline const Visualization::VListStyle& VMethodStyle::typeArguments() const { return typeArguments_; }
inline const Visualization::VListStyle& VMethodStyle::results() const { return results_; }

}
