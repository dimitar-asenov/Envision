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

#include "PythonSet.h"


namespace InformationScripting {

namespace helper {

using namespace boost;

PythonSetBase::PythonSetBase() : python::object(python::detail::new_reference(PySet_New(0)))
{}

void PythonSetBase::add(object_cref elem)
{
	if (PyAnySet_CheckExact(ptr()))
		 PySet_Add(ptr(), elem.ptr());
	else
		attr("add")(elem);
}

void PythonSetBase::discard(const python::object& elem)
{
	if (PyAnySet_CheckExact(ptr()))
		PySet_Discard(ptr(), elem.ptr());
	else
		attr("discard")(elem);
}

static struct register_set_pytype_ptr
{
	 register_set_pytype_ptr()
	 {
		  const_cast<python::converter::registration &>(
				python::converter::registry::lookup(python::type_id<PythonSet>())
			).m_class_object = &PySet_Type;
	 }
} register_set_pytype_ptr_;

}

}
