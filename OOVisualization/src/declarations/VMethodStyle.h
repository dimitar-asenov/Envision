/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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
#include "VisualizationBase/src/layouts/SequentialLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VMethodStyle : public Super<Visualization::DeclarativeItemBaseStyle>
{
	public:
		virtual ~VMethodStyle() override;

		Property<Visualization::TextStyle> name{this, "name"};
		Property<Visualization::ItemStyle> titleBackground{this, "titleBackground"};
		Property<Visualization::StaticStyle> defaultIcon{this, "defaultIcon"};
		Property<Visualization::StaticStyle> constructorIcon{this, "constructorIcon"};
		Property<Visualization::StaticStyle> destructorIcon{this, "destructorIcon"};
		Property<Visualization::StaticStyle> conversionIcon{this, "conversionIcon"};
		Property<Visualization::StaticStyle> operatorOverloadIcon{this, "operatorOverloadIcon"};
		Property<Visualization::SequentialLayoutStyle> header{this, "header"};
		Property<Visualization::LineStyle> signatureLine{this, "signatureLine"};
		Property<Visualization::VListStyle> annotations{this, "annotations"};
		Property<Visualization::StaticStyle> annotationsIcon{this, "annotationsIcon"};
		Property<Visualization::SequentialLayoutStyle> addons{this, "addons"};
		Property<Visualization::SequentialLayoutStyle> content{this, "content"};
		Property<Visualization::VListStyle> typeArguments{this, "typeArguments"};
		Property<Visualization::VListStyle> arguments{this, "arguments"};
		Property<Visualization::VListStyle> results{this, "results"};
		Property<Visualization::VListStyle> memberInitializers{this, "memberInitializers"};
		Property<Visualization::StaticStyle> throwsIcon{this, "throwsIcon"};
		Property<Visualization::VListStyle> throws{this, "throws"};
		Property<Visualization::VListStyle> metaCalls{this, "metaCalls"};
};

}
