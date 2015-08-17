/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "InformationEdge.h"

namespace InformationScripting {

const QString InformationEdge::COUNT_PROPERTY_{"count"};
const QString InformationEdge::NAME_PROPERTY_{"name"};

InformationEdge::InformationEdge(InformationNode* from, InformationNode* to, const QString& name,
											Orientation orientation)
	: PropertyMap{{{COUNT_PROPERTY_, 1}, {NAME_PROPERTY_, name}}}, from_{from}, to_{to}, orientation_{orientation}
{}

InformationEdge::InformationEdge(const InformationEdge& other)
	: PropertyMap(other)
{
	orientation_ = other.orientation_;
}

void InformationEdge::incrementCount()
{
	Property& count = (*this)[COUNT_PROPERTY_];
	count = static_cast<int>(count) + 1;
}

void InformationEdge::setFrom(InformationNode* from)
{
	Q_ASSERT(!from_);
	from_ = from;
}

void InformationEdge::setTo(InformationNode* to)
{
	Q_ASSERT(!to_);
	to_ = to;
}

} /* namespace InformationScripting */
