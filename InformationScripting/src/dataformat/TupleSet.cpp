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

#include "TupleSet.h"

namespace InformationScripting {

QSet<Tuple> TupleSet::tuples(const QString& tag) const
{
	if (tag.isEmpty()) return tuples_;
	QSet<Tuple> result;
	for (auto t : tuples_)
		if (t.get(0).first == tag) result.insert(t);
	return result;
}

QSet<Tuple> TupleSet::tuples(TupleSet::TupleCondition condition) const
{
	if (!condition) return tuples_;

	QSet<Tuple> result;
	for (auto t : tuples_)
		if (condition(t)) result.insert(t);
	return result;
}

void TupleSet::add(const Tuple& t)
{
	tuples_.insert(t);
}

void TupleSet::remove(const Tuple& t)
{
	tuples_.remove(t);
}

void TupleSet::remove(const TupleSet& tuples)
{
	tuples_.subtract(tuples.tuples_);
}

void TupleSet::unite(const TupleSet& with)
{
	tuples_.unite(with.tuples_);
}

} /* namespace InformationScripting */
