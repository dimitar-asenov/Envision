/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#pragma once

#include "cppexport_api.h"

#include "CodeUnit.h"
#include "SpecialCases.h"

#include "OOModel/src/declarations/Class.h"

namespace Export {
	class CompositeFragment;
}

namespace CppExport {

class CPPEXPORT_API CodeComposite
{
	public:
		CodeComposite(const QString& name);

		const QString& name() const;
		const QList<CodeUnit*>& units() const;
		void addUnit(CodeUnit* unit);

		void fragments(Export::SourceFragment*& header, Export::SourceFragment*& source);

		QString headerPartExtension();
		QString sourcePartExtension();

		bool isTemplateImplementationSeparateFile();

		bool isXMacroData();
		bool isXMacroInstantiation();

		QList<CodeUnit*>& additionalDependencies();

	private:
		const QString name_;
		QList<CodeUnit*> units_;
		QList<CodeUnit*> additionalUnitsThisDependsOn_;

		Export::SourceFragment* partFragment(CodeUnitPart* (CodeUnit::*part) ());
		Export::SourceFragment* headerFragment();
		Export::SourceFragment* sourceFragment();

		QSet<DependencyTarget> reduceSoftDependencies(QSet<CodeComposite*> hardDependencies,
																QSet<DependencyTarget> softDependencies);
		QString relativePath(CodeComposite* other);
		static Export::SourceFragment* addPragmaOnce(Export::SourceFragment* fragment);

		void sortUnits(CodeUnitPart*(CodeUnit::*part)(),
							std::function<QSet<CodeUnitPart*>(CodeUnitPart*)> dependencies);
		Export::CompositeFragment* addNamespaceFragment(Export::CompositeFragment* parentFragment,
																		OOModel::Module* namespaceNode);

		/**
		 * calculate code units in this composite which have non empty 'part'
		 */
		QList<CodeUnit*> nonEmptyUnits(CodeUnitPart*(CodeUnit::*part)());

		/**
		 * calculate all hard dependencies of this composite 'part'
		 */
		QSet<CodeComposite*> calculateDependencies(CodeUnitPart* (CodeUnit::*part)());

		Export::CompositeFragment* printHardDependencies(CodeUnitPart* (CodeUnit::*part)(),
																		 QSet<CodeComposite*> hardDependencies);

		/**
		 * all forward declarations in header parts of this composite's units.
		 */
		QSet<Model::Node*> softDependenciesInHeaderParts();

		/**
		 * group forward declarations by namespace and prints them in fragment
		 */
		void printRemainingSoftDependencies(QList<OOModel::Class*> remainingSoftDependencies,
														Export::CompositeFragment* fragment);
		Export::CompositeFragment* printForwardDeclaration(OOModel::Class* classs);
};

inline const QString& CodeComposite::name() const { return name_; }
inline const QList<CodeUnit*>& CodeComposite::units() const { return units_; }
inline QString CodeComposite::headerPartExtension()
{
	return ".h";
}
inline QString CodeComposite::sourcePartExtension()
{
	if (isTemplateImplementationSeparateFile()) return ".hpp";
	return ".cpp";
}

inline Export::SourceFragment* CodeComposite::headerFragment()
{
	if (isXMacroData())
		return partFragment(&CodeUnit::headerPart);
	if (isXMacroInstantiation())
		return addPragmaOnce(SpecialCases::includeXMacroData(this, partFragment(&CodeUnit::headerPart), false));
	return addPragmaOnce(partFragment(&CodeUnit::headerPart));
}
inline Export::SourceFragment* CodeComposite::sourceFragment()
{
	if (isTemplateImplementationSeparateFile())
		return addPragmaOnce(partFragment(&CodeUnit::sourcePart));
	if (isXMacroInstantiation())
		return addPragmaOnce(SpecialCases::includeXMacroData(this, partFragment(&CodeUnit::sourcePart), true));
	return partFragment(&CodeUnit::sourcePart);
}

inline bool CodeComposite::isTemplateImplementationSeparateFile()
{
	for (auto unit : units_)
		if (unit->isTemplateImplementationSeparateFile())
			return true;
	return false;
}

inline bool CodeComposite::isXMacroData()
{
	return name_.endsWith(SpecialCases::XMACRO_DATA_FILENAME);
}

inline bool CodeComposite::isXMacroInstantiation()
{
	return name_.endsWith(SpecialCases::XMACRO_INSTANTIATION_FILENAME);
}

inline QList<CodeUnit*>& CodeComposite::additionalDependencies()
{ return additionalUnitsThisDependsOn_; }

}
