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

#include "../cppexport_api.h"

#include "DependencyTarget.h"

namespace Model {
	class Node;
}

namespace OOModel {
	class ReferenceExpression;
}

namespace CppExport {

class DependencyComposite;

class CPPEXPORT_API DependencyUnit
{
	public:
		DependencyUnit(QString name, Model::Node* node);

		const QString& name() const;

		QSet<DependencyUnit*> dependencies() const;
		void calculateDependencies(QList<DependencyUnit*>& allUnits);

		DependencyComposite* composite() const;
		void setComposite(DependencyComposite* composite);

	private:
		QString name_;
		Model::Node* node_{};
		DependencyComposite* composite_{};
		QList<DependencyTarget> targets_;
		QSet<DependencyUnit*> dependencies_;

		static QList<DependencyTarget> calculateTargets(Model::Node* node);
		static bool isNameOnlyDependency(OOModel::ReferenceExpression* reference);
		static Model::Node* fixedTarget(OOModel::ReferenceExpression* referenceExpression);
};

inline const QString& DependencyUnit::name() const { return name_; }

inline DependencyComposite* DependencyUnit::composite() const { return composite_; }

inline void DependencyUnit::setComposite(DependencyComposite* composite) { composite_ = composite; }

inline QSet<DependencyUnit*> DependencyUnit::dependencies() const { return dependencies_; }

}
