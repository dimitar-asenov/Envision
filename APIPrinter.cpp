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

#include "APIPrinter.h"

#include "Config.h"

APIPrinter::APIPrinter(QTextStream& outStream, const APIData& data) : out_{outStream}, data_{data}
{
	maxLineLength_ = Config::instance().maxLineLength();
}

void APIPrinter::print()
{
	out_ << "// GENERATED FILE: CHANGES WILL BE LOST!" << endl;
	out_ << endl;
	printHeaders();
	out_ << "namespace InformationScripting {" << endl << endl;
	out_ << "using namespace boost::python;" << endl << endl;
	out_ << "BOOST_PYTHON_MODULE(AstApi) {" << endl << endl;
	printClasses();
	printTypedListWrappers();
	out_ << endl << "}" << endl;
	out_ << endl << "} /* namespace InformationScripting */" << endl;
}

void APIPrinter::printHeaders()
{
	for (QString includePath : data_.includePaths_)
		out_ << "#include \"" << includePath << "\"" << endl;
	out_ << endl;
}

void APIPrinter::printClasses()
{
	for (const auto& cData : data_.classes()) printClass(cData);
}

void APIPrinter::printClass(const ClassData& cData)
{
	// each class gets it's own scope:
	out_ << "{" << endl;
	indent();
	QString classString;
	if (cData.enums_.size()) // We only need a scope variable if we have enums
		classString = QString("scope %1scope = ").arg(cData.className_);
	classString += "class_<" + cData.qualifiedName_;
	// print bases:
	if (cData.baseClasses_.size())
		classString += ", bases<" + cData.baseClasses_.join(", ") + ">";
	else if (cData.abstract_)
		classString += ", boost::noncopyable";
	classString += ">";
	// print name:
	classString += "(\"" + cData.className_ + "\"";
	// print constructors: // TODO
	if (cData.abstract_) // abstract classes have no constructor
		classString += ", no_init";
	classString += ")";
	printPossiblyLongString(classString);

	// printProperties
	indent();
	int propertyCount = cData.attributes_.size();
	if (propertyCount > 0)
	{
		out_ << endl;
		for (int i = 0; i < propertyCount - 1; ++i)
		{
			printAttribute(cData.attributes_[i]);
			out_ << endl;
		}
		printAttribute(cData.attributes_[propertyCount-1]);
	}
	out_ << ";" << endl;
	unIndent();

	printEnumsOfClass(cData);
	// close scope
	unIndent();
	out_ << "}" << endl << endl;
}

void APIPrinter::printEnumsOfClass(const ClassData& cData)
{
	for (const auto& eData : cData.enums_) printEnum(eData);
}

void APIPrinter::printEnum(const EnumData& eData)
{
	out_ << indent_ <<"enum_<" << eData.qualifiedName_ << ">(\"" << eData.enumName_ << "\")";
	indent();
	int valueCount = eData.values_.size();
	if (valueCount > 0)
	{
		out_ << endl;
		for (int i = 0; i < valueCount - 1; ++i)
		{
			auto enumConstant = eData.values_[i];
			QString enumValue = ".value(\"" + enumConstant.first + "\", " + enumConstant.second + ")";
			printPossiblyLongString(enumValue);
			out_ << endl;
		}
		auto enumConstant = eData.values_[valueCount-1];
		QString enumValue = ".value(\"" + enumConstant.first + "\", " + enumConstant.second + ")";
		printPossiblyLongString(enumValue, 1);
	}
	out_ << ";" << endl;
	unIndent();
}

void APIPrinter::printAttribute(const ClassAttribute& attr)
{
	out_ << indent_ << ".add_property(\"" << attr.name_ << "\"," << endl;
	indent();
	printPossiblyLongString(attr.getterQualified_ + ",");
	out_ << endl;
	out_ << indent_ << attr.setterQualified_ << ")";
	unIndent();
}

void APIPrinter::printTypedListWrappers()
{
	auto typedLists = data_.typedLists();
	if (typedLists.empty()) return;

	out_ << "{" << endl;
	indent();
	for (auto it = typedLists.begin(); it != typedLists.end(); ++it)
	{
		out_ << indent_ << "using " << it.value() << " = " << it.key() << ";" << endl;
		out_ << indent_ << "class_<" << it.key() << ">" << "(\"" << it.value() << "\")" << endl;
		indent();
		out_ << indent_ << ".def(\"__len__\", &" << it.value() << "::size)" << endl;
		out_ << indent_ << ".def(\"__iter__\", iterator<" << it.value() << ", return_internal_reference<>>())";
		out_ << ";" << endl << endl;
		unIndent();
	}
	unIndent();
	out_ << "}" << endl;
}

void APIPrinter::printPossiblyLongString(const QString& data, int additionalLength)
{
	if (data.length() + indent_.length() + additionalLength > maxLineLength_)
	{
		int commaIndex = data.indexOf(',');
		QString firstPart = data.mid(0, commaIndex + 1);
		QString secondPart = data.mid(commaIndex + 1);
		out_ << indent_ << firstPart << endl;
		indent();
		out_ << indent_ << secondPart;
		unIndent();
	}
	else
	{
		out_ << indent_ << data;
	}
}

void APIPrinter::indent()
{
	indent_.append('\t');
}

void APIPrinter::unIndent()
{
	indent_.remove(indent_.size()-1, 1);
}
