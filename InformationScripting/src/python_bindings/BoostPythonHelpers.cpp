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

#include "BoostPythonHelpers.h"

#include "PythonSet.h"
#include "../dataformat/Property.h"
#include "../dataformat/Tuple.h"
#include "../dataformat/TupleSet.h"
#include "../query_framework/QueryParsingException.h"
#include "../query_framework/QueryRuntimeException.h"

#include "OOModel/src/declarations/Class.h"
#include "ModelBase/src/nodes/Node.h"

namespace InformationScripting {

namespace PythonConverters {

using namespace boost;

struct QString_to_python_str
{
		static PyObject* convert(QString s)
		{
			return python::incref(python::object(s.toLatin1().constData()).ptr());
		}
};

struct QString_from_python_str
{
		QString_from_python_str()
		{
			python::converter::registry::push_back(&convertible, &construct, python::type_id<QString>());
		}

		// Determine if objPtr can be converted in a QString
		static void* convertible(PyObject* objPtr)
		{
			if (!PyUnicode_Check(objPtr)) return 0;
			return objPtr;
		}

		// Convert objPtr into a QString
		static void construct(PyObject* objPtr, python::converter::rvalue_from_python_stage1_data* data)
		{
			// Extract the character data from the python string
			python::handle<> rawBytesHandle(PyUnicode_AsUTF8String(objPtr));
			Q_ASSERT(rawBytesHandle);

			// Grab pointer to memory into which to construct the new QString
			void* storageBytes = ((python::converter::rvalue_from_python_storage<QString>*) data)->storage.bytes;

			// in-place construct the new QString using the character data extraced from the python object
			new (storageBytes) QString{PyBytes_AsString(rawBytesHandle.get())};
			data->convertible = storageBytes;
		}
};

template<typename T>
struct QSet_to_python_set
{
		static PyObject* convert(QSet<T> toConvert)
		{
			helper::PythonSet pySet;
			for (auto val : toConvert) pySet.add(val);
			return python::incref(pySet.ptr());
		}
};

// Specialization for pointer sets:
template<typename T>
struct QSet_to_python_set<T*>
{
		static PyObject* convert(QSet<T*> toConvert)
		{
			helper::PythonSet pySet;
			for (auto val : toConvert) pySet.add(python::ptr(val));
			return python::incref(pySet.ptr());
		}
};

template<typename T>
struct QList_to_python_list
{
		static PyObject* convert(QList<T> toConvert)
		{
			python::list list;
			for (auto val : toConvert) list.append(val);
			return python::incref(list.ptr());
		}
};

// Specialization for pointer lists:
template<typename T>
struct QList_to_python_list<T*>
{
		static PyObject* convert(QList<T*> toConvert)
		{
			python::list list;
			for (auto val : toConvert) list.append(python::ptr(val));
			return python::incref(list.ptr());
		}
};

}

// Method adapted from: https://github.com/josephturnerjr/boost-python-tutorial/blob/master/part2/handle_error.cpp
// Parses the value of the active python exception
QString BoostPythonHelpers::parsePythonException()
{
	using namespace boost;

	PyObject *typePtr = nullptr, *valuePtr = nullptr, *tracebackPtr = nullptr;
	// Fetch the exception info from the Python C API
	PyErr_Fetch(&typePtr, &valuePtr, &tracebackPtr);

	QString errorMessage{"Unfetchable Python error"};
	if (typePtr != nullptr)
	{
		python::handle<> typePtrHandle(typePtr);
		python::extract<QString> typePtrString(python::str{typePtrHandle});
		if (typePtrString.check()) errorMessage = typePtrString();
		else errorMessage = "Unknown exception type";
	}
	if (valuePtr != nullptr)
	{
		python::handle<> valuePtrHandle(valuePtr);
		python::extract<QString> valuePtrString(python::str{valuePtrHandle});
		if (valuePtrString.check()) errorMessage += ":<br>" + valuePtrString();
		else errorMessage += ":<br>Unparseable Python error: ";
	}
	// Parse lines from the traceback using the Python traceback module
	if (tracebackPtr != nullptr)
	{
		python::handle<> tracebackPtrHandle(tracebackPtr);
		// Load the traceback module and the format_tb function
		python::object tb(python::import("traceback"));
		python::object formatTraceback(tb.attr("format_tb"));
		// Call the format_tb function to get the traceback strings.
		python::object tracebakList(formatTraceback(tracebackPtrHandle));
		python::object tracebackString(python::str("<br>").join(tracebakList));

		python::extract<QString> tracebackErrorString(tracebackString);
		if (tracebackErrorString.check()) errorMessage += ":<br>" + tracebackErrorString();
		else errorMessage += ":<br>Unparseable Python traceback";
	}
	return errorMessage;
}

void BoostPythonHelpers::initializeConverters()
{
	using namespace boost;

	// register the to-python converters
	python::to_python_converter<QString, PythonConverters::QString_to_python_str>();

	python::to_python_converter<QSet<Tuple>, PythonConverters::QSet_to_python_set<Tuple>>();
	python::to_python_converter<QSet<OOModel::Class*>, PythonConverters::QSet_to_python_set<OOModel::Class*>>();

	python::to_python_converter<QList<QString>, PythonConverters::QList_to_python_list<QString>>();
	python::to_python_converter<QList<TupleSet>, PythonConverters::QList_to_python_list<TupleSet>>();
	python::to_python_converter<QList<NamedProperty>, PythonConverters::QList_to_python_list<NamedProperty>>();
	python::to_python_converter<QList<Model::Node*>, PythonConverters::QList_to_python_list<Model::Node*>>();

	// register the from-python converters
	PythonConverters::QString_from_python_str();

	// register exceptions translators:
	python::register_exception_translator<QueryParsingException>(&BoostPythonHelpers::translate<QueryParsingException>);
	python::register_exception_translator<QueryRuntimeException>(&BoostPythonHelpers::translate<QueryRuntimeException>);
}

template <typename Exception>
void BoostPythonHelpers::translate(const Exception& e)
{
	PyErr_SetString(PyExc_RuntimeError, e.message().toLatin1().data());
}

}
