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
 * Text.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODEL_TEXT_H_
#define MODEL_TEXT_H_

#include "Node.h"
#include "nodeMacros.h"

namespace Model {

class NameChange;
template<class T> class FieldSet;

class MODELBASE_API Text: public Node
{
	friend class NameChange;

	NODE_DECLARE_STANDARD_METHODS(Text)

	private:
		QString text;

		FieldSet<QString>* getSetCommand(const QString& newText);
	public:

		Text(const QString& text);
		const  QString&  get() const;
		operator const QString&() const;

		void set(const QString &newText);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
};

inline const QString& Text::get() const { return text; }
inline Text::operator const QString&() const { return text; }

}

#endif /* MODEL_TEXT_H_ */
