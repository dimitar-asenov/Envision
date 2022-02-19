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

APIPrinter::APIPrinter()
{
	maxLineLength_ = Config::instance().maxLineLength();
}

void APIPrinter::print()
{
	printClasses();
	printOneFile("TypedListWrappers.cpp", [this](){printTypedListWrappers();});
	printOneFile("init.cpp", [this](){printInitFunctionCalls();});
}

void APIPrinter::printLicense()
{
	auto licensePath = Config::instance().envisionReadPath() + "LICENSE";
	QFile licenseFile(licensePath);
	bool open = licenseFile.open(QIODevice::ReadOnly);
	Q_ASSERT(open);
	out_ << licenseFile.readAll() << Qt::endl;
}

void APIPrinter::printHeaders()
{
	for (const QString& includePath : APIData::instance().includePaths_)
		out_ << QString("#include \"%1\"").arg(includePath) << Qt::endl;
	out_ << Qt::endl;
}

void APIPrinter::printInitFunctionCalls()
{
	// print all extern forward declarations
	out_ << "extern void initTypedListWrappers();" << Qt::endl << Qt::endl;
	for (const auto& cData : APIData::instance().classes())
		out_ << indent_ << "extern void initClass" << cData.className_ << "();" << Qt::endl;

	out_ << Qt::endl << Qt::endl;
	out_ << "BOOST_PYTHON_MODULE(AstApi) {" << Qt::endl << Qt::endl;

	indent();

	// Print classes
		for (const auto& cData : APIData::instance().classes())
		    out_ << indent_ << "initClass" << cData.className_ << "();" << Qt::endl;

	// TypedListWrappers
		out_ << indent_ << "initTypedListWrappers();" << Qt::endl;

	unIndent();
	out_ << "}" << Qt::endl << Qt::endl;
}

void APIPrinter::printClasses()
{
	for (const auto& cData : APIData::instance().classes())
		printOneFile(QString{cData.qualifiedName_}.replace(QRegularExpression{"[:<>]"}, "_")+".cpp",
						 [this, &cData](){printClass(cData);});
}

void APIPrinter::printClass(const ClassData& cData)
{
	// each class gets it's own scope:
	out_ << "void initClass" << cData.className_ <<"() {" << Qt::endl;
	indent();
	if (!cData.usingAlias_.isNull())
		out_ << indent_ << "using " << cData.usingAlias_ << " = " << cData.qualifiedName_ << ";" << Qt::endl;

	for (const auto& overload : cData.overloadAliases_)
		printOverload(overload);

	// print bases:
	QString basesString = "";
	if (cData.baseClasses_.size())
		basesString = QString(", bases<%1>").arg(cData.baseClasses_.join(", "));
	else if (cData.abstract_)
		basesString = ", boost::noncopyable";

	QString noInit = "";
	if (cData.abstract_) noInit = ", no_init";
	QString classString = QString("auto aClass = class_<%1%2>(\"%3\"%4);").arg(cData.qualifiedName_.trimmed(),
																		  basesString, cData.className_, noInit);

	printPossiblyLongString(classString);

	indent();
	for (const auto& attribute : cData.attributes_)
		printAttribute(attribute);
	for (const auto& method : cData.methods_)
		printMethod(method);
	out_ << Qt::endl;
	unIndent();

	// We only need a scope variable if we have enums
	if (cData.enums_.size())
	{
		printPossiblyLongString( QString("scope %1Scope = aClass;\n").arg(cData.className_) );
		printEnumsOfClass(cData);
	}

	// close scope
	unIndent();
	out_ << "}" << Qt::endl << Qt::endl;
}

void APIPrinter::printOverload(const OverloadDescriptor& overload)
{
	printPossiblyLongString(QString("%1 = %2;\n").arg(overload.signature_, overload.functionAddress_));
}

void APIPrinter::printEnumsOfClass(const ClassData& cData)
{
	for (const auto& eData : cData.enums_) printEnum(eData);
}

void APIPrinter::printEnum(const EnumData& eData)
{
	out_ << indent_ <<"enum_<" << eData.qualifiedName_ << ">(\"" << eData.enumName_ << "\")";
	indent();
	for (const auto& enumConstant : eData.values_)
	{
		out_ << Qt::endl;
		printPossiblyLongString(QString(".value(\"%1\", %2)").arg(enumConstant.first, enumConstant.second));
	}
	out_ << ";" << Qt::endl;
	unIndent();
}

void APIPrinter::printAttribute(const ClassAttribute& attr)
{
	out_ << Qt::endl << indent_ << "aClass.add_property(\"" << attr.name_ << "\"," << Qt::endl;
	indent();
	printPossiblyLongString(attr.getterQualified_ + ",");
	out_ << Qt::endl;
	out_ << indent_ << attr.setterQualified_ << ");";
	unIndent();
}

void APIPrinter::printMethod(const ClassMethod& method)
{
	out_ << Qt::endl;
	printPossiblyLongString(QString("aClass.def(\"%1\", %2);").arg(method.name_, method.wrappedFunctionPointer_));
	if (method.static_)
		out_ << Qt::endl << indent_ << "aClass.staticmethod(\"" << method.name_ << "\");";
}

void APIPrinter::printTypedListWrappers()
{
	auto typedLists = APIData::instance().typedLists();
	if (typedLists.empty()) return;

	out_ << "void initTypedListWrappers() {" << Qt::endl;
	indent();
	for (auto it = typedLists.begin(); it != typedLists.end(); ++it)
	{
		QString name = it.value() + "Def";
		out_ << indent_ << "using " << it.value() << " = " << it.key() << ";" << Qt::endl;
		printPossiblyLongString("auto " + name + " = class_<" + it.key() + ", bases<Model::List>>" + "(\"" + it.value()
			  + "\");\n");
		indent();
		out_ << indent_ << name << ".def(\"__len__\", &" << it.value() << "::size);" << Qt::endl;
		out_ << indent_ << name << ".def(\"__iter__\", iterator<" << it.value() << ", return_internal_reference<>>());";
		out_ << Qt::endl << Qt::endl;
		unIndent();
	}
	unIndent();
	out_ << "}" << Qt::endl << Qt::endl;
}

void APIPrinter::printPossiblyLongString(const QString& data, int additionalLength)
{
	static const QList<QChar> splitSymbols{' ', ',', '(', '='};
	if (data.length() + indent_.length() + additionalLength > maxLineLength_)
	{
		int mid = maxLineLength_ / 2;
		int offset = 1;
		// Search for a position nearest to mid to split:
		while (std::abs(offset) < mid)
		{
			if (splitSymbols.contains(data.at(mid + offset))) break;
			if (offset > 0) offset *= -1;
			else offset = offset * -1 + 1;
		}
		int splitIndex = mid + offset;
		QString firstPart = data.mid(0, splitIndex + 1);
		QString secondPart = data.mid(splitIndex + 1);

		printPossiblyLongString(firstPart.trimmed() + "\n");
		printPossiblyLongString("\t" + secondPart);
	}
	else
	{
		out_ << indent_ << data;
	}
}

void APIPrinter::printOneFile(const QString& filename, PrintMethod method)
{
	// Create out file
	outFile_.setFileName(Config::instance().exportPath() + filename);
	if (!outFile_.open(QIODevice::WriteOnly | QIODevice::Text)) Q_ASSERT(false);
	out_.setDevice(&outFile_);
	maxLineLength_ = Config::instance().maxLineLength();

	printLicense();
	out_ << "// GENERATED FILE: CHANGES WILL BE LOST!" << Qt::endl << Qt::endl;
	out_ << "#include \"../AstApi.h\"" << Qt::endl << Qt::endl;
	printHeaders();
	out_ << "#include \"ModelBase/src/persistence/ClipboardStore.h\"" << Qt::endl << Qt::endl;
	out_ << "#include \"ModelBase/src/commands/UndoCommand.h\"" << Qt::endl << Qt::endl;
	out_ << "namespace InformationScripting {" << Qt::endl << Qt::endl;
	out_ << "using namespace boost::python;" << Qt::endl << Qt::endl;

	method();

	out_ << Qt::endl << "} /* namespace InformationScripting */" << Qt::endl;
	outFile_.close();
}

void APIPrinter::indent()
{
	indent_.append('\t');
}

void APIPrinter::unIndent()
{
	indent_.remove(indent_.size()-1, 1);
}
