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
 * HText.h
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HTEXT_H_
#define HTEXT_H_

#include "../interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HText : public GenericHandler
{
	public:
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);

		virtual void mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
		virtual void focusOutEvent(Visualization::Item *target, QFocusEvent *event);
		static HText* instance();

	protected:
		HText();

		void erase(Visualization::Item *target, bool forwards, bool onlyDeleteIfSelected);
		void insertText(Visualization::Item *target, const QString& textToInsert);

	private:
		bool doubleClick;
};

}

#endif /* HTEXT_H_ */
