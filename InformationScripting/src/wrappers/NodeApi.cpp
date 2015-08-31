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

#include "NodeApi.h"

#include "../graph/PropertyMap.h"
#include "../graph/InformationNode.h"
#include "../graph/InformationEdge.h"
#include "../graph/Graph.h"

namespace InformationScripting {

using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(addEdge_overloads, Graph::addEdge, 3, 5)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(nodes_overloads, Graph::nodes, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(edges_overloads, Graph::edges, 0, 1)

BOOST_PYTHON_MODULE(NodeApi) {
		class_<PropertyMap, boost::noncopyable>("PropertyMap", no_init)
				.def("__getattr__", &PropertyMap::pythonAttribute);
		class_<InformationNode, bases<PropertyMap>>("InformationNode", no_init);
		class_<InformationEdge, bases<PropertyMap>>("InformationEdge", no_init);

		void (Graph::*remove1)(InformationNode*)        = &Graph::remove;
		void (Graph::*remove2)(QList<InformationNode*>) = &Graph::remove;

		class_<Graph>("Graph")
				.def("add", &Graph::add, return_value_policy<reference_existing_object>())
				.def("addEdge", &Graph::addEdge, addEdge_overloads()[return_value_policy<reference_existing_object>()])
				.def("remove", remove1)
				.def("remove", remove2)
				.def("nodes", &Graph::nodes, nodes_overloads())
				.def("edges", &Graph::edges, edges_overloads());
}

} /* namespace InformationScripting */
