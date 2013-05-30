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

#include "../interactionbase_api.h"

namespace Interaction {

class INTERACTIONBASE_API OperatorDescriptor {
	public:
		enum Associativity {LeftAssociative, RightAssociative, NotAssociative};

		OperatorDescriptor();
		OperatorDescriptor(const QString& name, const QString& signature, int precedence, Associativity associativity);
		virtual ~OperatorDescriptor();

		// A transient operator descriptor is one that is created dynamically and is associated with a particular
		// instance of an Operator. If the operator is deleted the associated transitive descriptor is no longer in use
		// and should also be deleted. Transient descriptors are used when creating operators to represent errors.
		void setTransient(bool transient);

		const QString& name();
		const QStringList& signature();
		int numOperands();
		int precedence();
		Associativity associativity();
		bool isTransient();
		const QString& prefix();
		const QStringList& infixes();
		const QString& infix(int at);
		const QString& postfix();

		QStringList delimiters();

	private:
		QString name_;
		QStringList signature_;
		int num_operands_{};
		int precedence_{};
		Associativity associativity_{};
		bool transient_{};

		// Based on the signature we precompute the prefix, infixes and postfix to speed up later processing
		QString prefix_;
		QStringList infixes_;
		QString postfix_;
};

inline void OperatorDescriptor::setTransient(bool transient) { transient_ = transient; }

inline const QString& OperatorDescriptor::name() { return name_; }
inline const QStringList& OperatorDescriptor::signature() { return signature_; }
inline int OperatorDescriptor::numOperands() { return num_operands_; }
inline int OperatorDescriptor::precedence() { return precedence_; }
inline OperatorDescriptor::Associativity OperatorDescriptor::associativity() { return associativity_; }
inline bool OperatorDescriptor::isTransient() { return transient_; }
inline const QString& OperatorDescriptor::prefix() { return prefix_; }
inline const QStringList& OperatorDescriptor::infixes() { return infixes_; }
inline const QString& OperatorDescriptor::infix(int at) { return infixes_.at(at); }
inline const QString& OperatorDescriptor::postfix() { return postfix_; }

}
