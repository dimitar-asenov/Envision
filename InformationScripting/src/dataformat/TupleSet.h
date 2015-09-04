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

#pragma once

#include "../informationscripting_api.h"

#include "Tuple.h"

namespace InformationScripting {

class INFORMATIONSCRIPTING_API TupleSet
{
	public:
		using TupleCondition = std::function<bool (const Tuple& t)>;

		template<class Condition>
		QSet<Tuple> tuples(Condition condition = {}) const;
		/**
		 * Returns all tuples which are tagged with \a tag.
		 */
		QSet<Tuple> tuples(const QString& tag) const;
		QSet<Tuple> tuples(const char* tag) const;

		QSet<Tuple> tuples() const;

		void add(const Tuple& t);
		void remove(const Tuple& t);
		void remove(const TupleSet& tuples);
		void unite(const TupleSet& with);

	private:
		QSet<Tuple> tuples_;
};

template <class Condition>
inline QSet<Tuple> TupleSet::tuples(Condition condition) const
{
	if (!condition) return tuples_;

	QSet<Tuple> result;
	for (auto t : tuples_)
		if (condition(t)) result.insert(t);
	return result;
}

inline QSet<Tuple> TupleSet::tuples(const char* tag) const { return tuples(QString(tag)); }
inline QSet<Tuple> TupleSet::tuples() const { return tuples_; }

inline void TupleSet::add(const Tuple& t) { tuples_.insert(t); }
inline void TupleSet::remove(const Tuple& t) { tuples_.remove(t); }
inline void TupleSet::remove(const TupleSet& tuples) { tuples_.subtract(tuples.tuples_); }
inline void TupleSet::unite(const TupleSet& with) { tuples_.unite(with.tuples_); }

} /* namespace InformationScripting */
