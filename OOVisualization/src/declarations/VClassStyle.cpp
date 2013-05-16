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

#include "VClassStyle.h"

namespace OOVisualization {

void VClassStyle::load(Visualization::StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("nameDefault", nameDefault_);
	sl.load("namePublic", namePublic_);
	sl.load("namePrivate", namePrivate_);
	sl.load("nameProtected", nameProtected_);
	sl.load("icon", icon_);
	sl.load("header", header_);
	sl.load("typeArguments", typeArguments_);
	sl.load("baseClasses", baseClasses_);
	sl.load("annotations", annotations_);
	sl.load("enumerators", enumerators_);
	sl.load("friendsSymbol", friendsSymbol_);
	sl.load("friends", friends_);
	sl.load("body", body_);
	sl.load("content", content_);
	sl.load("fieldContainer", fieldContainer_);
	sl.load("publicFieldArea", publicFieldArea_);
	sl.load("privateFieldArea", privateFieldArea_);
	sl.load("protectedFieldArea", protectedFieldArea_);
	sl.load("defaultFieldArea", defaultFieldArea_);
}

}
