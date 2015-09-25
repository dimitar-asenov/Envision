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

#include "DataApi.h"

#include "../dataformat/Tuple.h"
#include "../dataformat/TupleSet.h"

#include "ModelBase/src/nodes/Node.h"

namespace InformationScripting {

using namespace boost::python;

object value(const NamedProperty& self) {
	return pythonObject(self.second);
}

object Tuple_getAttr(const Tuple& self, const QString& name) {
	return pythonObject(self[name]);
}

std::shared_ptr<Tuple> makeTuple(list args) {
	stl_input_iterator<NamedProperty> begin(args), end;
	return std::shared_ptr<Tuple>{new Tuple{QList<NamedProperty>::fromStdList(std::list<NamedProperty>(begin, end))}};
}

BOOST_PYTHON_MODULE(DataApi) {
		class_<NamedProperty>("NamedProperty", init<QString, QString>())
				.def(init<QString, Model::Node*>())
				.def_readwrite("name", &NamedProperty::first)
				.add_property("value", &value);

		class_<Tuple>("Tuple", init<>())
				.def("__init__", make_constructor(makeTuple))
				.def("tupleTag", &Tuple::tag)
				.def("add", &Tuple::add)
				.def("__getattr__", &Tuple_getAttr);

		QSet<Tuple> (TupleSet::*tuplesString)(const QString&) const = &TupleSet::tuples;
		QSet<Tuple> (TupleSet::*take1)(const QString&) = &TupleSet::take;
		void (TupleSet::*removeTuple)(const Tuple&) = &TupleSet::remove;

		class_<TupleSet>("TupleSet")
				.def("tuples", tuplesString)
				.def("take", take1)
				.def("remove", removeTuple)
				.def("add", &TupleSet::add);
}

} /* namespace InformationScripting */
