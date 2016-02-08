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
		TupleSet() = default;
		TupleSet(const QList<Tuple>& initialTuples);

		template<typename Condition>
		QSet<Tuple> tuples(Condition condition) const;
		/**
		 * Returns all tuples which are tagged with \a tag.
		 */
		QSet<Tuple> tuples(const QString& tag) const;
		QSet<Tuple> tuples(const char* tag) const;

		QSet<Tuple> tuples() const;

		void add(const Tuple& t);
		void remove(const Tuple& t);
		void remove(const TupleSet& tuples);
		QSet<Tuple> take(const QString& tag);
		QSet<Tuple> take(const char* tag);
		template<typename Condition>
		QSet<Tuple> take(Condition condition);
		QSet<Tuple> takeAll();
		void unite(const TupleSet& with);

		/**
		 * Adds all properties of type \a T as single Tuples with tag \a tag.
		 */
		template<typename T>
		void addPropertiesAsTuples(const QString& tag);

		bool isEmpty() const;

	private:
		QHash<QString, QSet<Tuple>> tuples_;
};

template <typename Condition>
inline QSet<Tuple> TupleSet::tuples(Condition condition) const
{
	QSet<Tuple> result;
	for (auto hashIt = tuples_.begin(); hashIt != tuples_.end(); ++hashIt)
		for (const auto& t : hashIt.value())
			if (condition(t)) result.insert(t);
	return result;
}

inline QSet<Tuple> TupleSet::tuples(const char* tag) const { return tuples(QString{tag}); }

inline void TupleSet::add(const Tuple& t) { tuples_[t.tag()].insert(t); }
inline void TupleSet::remove(const Tuple& t) { tuples_[t.tag()].remove(t); }

inline QSet<Tuple> TupleSet::take(const QString& tag) { return tuples_.take(tag); }
inline QSet<Tuple> TupleSet::take(const char* tag) { return tuples_.take(tag); }
template <typename Condition>
inline QSet<Tuple> TupleSet::take(Condition condition)
{
	QSet<Tuple> result;

	for (auto hashIt = tuples_.begin(); hashIt != tuples_.end(); ++hashIt)
	{
		auto& set = hashIt.value();
		auto setIt = set.begin();
		while (setIt != set.end())
		{
			if (condition(*setIt))
			{
				result.insert(*setIt);
				setIt = set.erase(setIt);
			}
			else ++setIt;
		}
	}
	return result;
}

template <typename T>
inline void TupleSet::addPropertiesAsTuples(const QString& tag)
{
	for (auto hashIt = tuples_.begin(); hashIt != tuples_.end(); ++hashIt)
	{
		if (hashIt.key() != tag)
		for (const auto& t : hashIt.value())
		{
			auto properties = t.valuesOfType<T>();
			for (const auto& p : properties)
				tuples_[tag].insert({{tag, p}});
		}
	}
}

inline bool TupleSet::isEmpty() const { return tuples_.isEmpty(); }

}
