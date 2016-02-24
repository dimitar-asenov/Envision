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

namespace OOModel {
	class ReferenceExpression;
	class Type;
}

namespace Model {
	class Node;
}

namespace Export {
	class SourceFragment;
}

namespace CppExport {

class CodeUnit;

class CPPEXPORT_API CodeUnitPart
{
	public:
		CodeUnitPart(CodeUnit* parent);

		CodeUnit* parent() const;

		Export::SourceFragment* fragment() const;
		/**
		 * returns whether there is at least one text fragment inside sourceFragment.
		 */
		bool isSourceFragmentEmpty() const;
		void setFragment(Export::SourceFragment* sourceFragment);

		const QSet<Model::Node*>& nameNodes() const;
		const QSet<OOModel::ReferenceExpression*>& referenceNodes() const;

		QSet<CodeUnitPart*> dependencies() const;
		QSet<Model::Node*> softDependencies() const;
		void calculateDependencies(QList<CodeUnit*>& allUnits);

		QSet<CodeUnitPart*> dependenciesWithinFile(QList<CodeUnit*> units);

	private:
		CodeUnit* parent_{};
		Export::SourceFragment* fragment_{};

		QSet<Model::Node*> nameNodes_;
		QSet<OOModel::ReferenceExpression*> referenceNodes_;
		QSet<Model::Node*> softTargets_;
		QSet<Model::Node*> hardTargets_;

		QSet<CodeUnitPart*> dependencies_;

		static bool isNameOnlyDependency(OOModel::ReferenceExpression* reference);
		static Model::Node* fixedTarget(OOModel::ReferenceExpression* referenceExpression);
		static const OOModel::Type* stripPointerOrReference(const OOModel::Type* type);

		/**
		 * checks whether the provided reference is "Type" in constructs of the form: (void)(Type*)nullptr;
		 * this construct is used to force dependencies when Envision's reference resolution system is not yet
		 * good enough to properly detect the dependency, causing the export to C++ to omit a necessary #include.
		 * if the structure matches the construct then the method asserts that no hard dependency on it has been
		 * found already so in case one forced dependency becomes redundant it can be detected more easily.
		 */
		void assertForcedDependencyNecessary(OOModel::ReferenceExpression* reference);
};

inline CodeUnit* CodeUnitPart::parent() const { return parent_; }

inline Export::SourceFragment* CodeUnitPart::fragment() const { return fragment_; }

inline const QSet<Model::Node*>& CodeUnitPart::nameNodes() const { return nameNodes_; }
inline const QSet<OOModel::ReferenceExpression*>& CodeUnitPart::referenceNodes() const { return referenceNodes_; }

inline QSet<CodeUnitPart*> CodeUnitPart::dependencies() const { return dependencies_; }
inline QSet<Model::Node*> CodeUnitPart::softDependencies() const { return softTargets_; }

}
