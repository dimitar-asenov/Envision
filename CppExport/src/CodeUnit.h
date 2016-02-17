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

#include "CodeUnitPart.h"
#include "ExportHelpers.h"
#include "Config.h"

#include "OOModel/src/declarations/Module.h"

namespace CppExport {

class CodeComposite;

class CPPEXPORT_API CodeUnit
{
	public:
		CodeUnit(QString name, Model::Node* node);

		const QString& name() const;
		Model::Node* node() const;
		CodeComposite* composite() const;
		void setComposite(CodeComposite* composite);

		CodeUnitPart* headerPart();
		CodeUnitPart* sourcePart();

		void calculateSourceFragments();
		void calculateDependencies(QList<CodeUnit*>& allUnits);

		OOModel::Module* nameSpace();

		bool hasNoHeaderPart();

		bool isTemplateImplementationSeparateFile();

	private:
		QString name_;
		Model::Node* node_{};
		CodeComposite* composite_{};
		bool hasNoHeaderPart_{};
		CodeUnitPart headerPart_{this};
		CodeUnitPart sourcePart_{this};
};

inline const QString& CodeUnit::name() const { return name_; }
inline Model::Node* CodeUnit::node() const { return node_; }
inline CodeComposite* CodeUnit::composite() const { return composite_; }
inline void CodeUnit::setComposite(CodeComposite* composite) { composite_ = composite; }
inline CodeUnitPart* CodeUnit::headerPart() { return &headerPart_; }
inline CodeUnitPart* CodeUnit::sourcePart() { return &sourcePart_; }
inline OOModel::Module* CodeUnit::nameSpace() { return ExportHelpers::parentNamespaceModule(node()); }
inline bool CodeUnit::hasNoHeaderPart() { return hasNoHeaderPart_; }

inline bool CodeUnit::isTemplateImplementationSeparateFile()
{ return Config::instance().separateTemplateImplementationSet().contains(name_); }


}
