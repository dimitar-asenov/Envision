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

#include "../interactionbase_api.h"

#include "VisualizationBase/src/items/Item.h"

namespace Interaction {

class INTERACTIONBASE_API CommandError
{
	public:
		CommandError(const QString& message = {}, Visualization::Item* visualization = nullptr);
		virtual ~CommandError();

		const QString& message();
		const QStringList& resolutionTips();
		Visualization::Item* visualization();

		void setMessage(const QString& message);
		void addResolutionTip(const QString& resolutionTip);
		void setVisualization(Visualization::Item* item);

	private:
		QString message_;
		QStringList resolutionTips_;
		Visualization::Item* visualization_;
};

inline const QString& CommandError::message() { return message_; }
inline const QStringList& CommandError::resolutionTips() { return resolutionTips_; }
inline Visualization::Item* CommandError::visualization() { return visualization_; }
inline void CommandError::setMessage(const QString& message) { message_ = message; }
inline void CommandError::addResolutionTip(const QString& resolutionTip) { resolutionTips_.append(resolutionTip); }
inline void CommandError::setVisualization(Visualization::Item* item) { visualization_ = item; }

}
