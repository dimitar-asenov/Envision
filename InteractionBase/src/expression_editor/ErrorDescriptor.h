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

#ifndef INTERACTIONBASE_ERRORDESCRIPTOR_H_
#define INTERACTIONBASE_ERRORDESCRIPTOR_H_

#include "../interactionbase_api.h"

#include "OperatorDescriptor.h"

namespace Interaction {

class INTERACTIONBASE_API ErrorDescriptor : public OperatorDescriptor
{
	public:
		ErrorDescriptor(const QString& prefix_text, const QString& postfix_text);

		const QString& errorPrefix() const;
		const QString& errorPostfix() const;
	private:
		QString errorPrefix_;
		QString errorPostfix_;
};

inline const QString& ErrorDescriptor::errorPrefix() const { return errorPrefix_; }
inline const QString& ErrorDescriptor::errorPostfix() const { return errorPostfix_; }

} /* namespace InteractionBase */
#endif /* INTERACTIONBASE_ERRORDESCRIPTOR_H_ */
