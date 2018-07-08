#pragma once

#include "cppimportlight_api.h"

#include "InteractionBase/src/commands/COpenDir.h"
#include "ModelBase/src/nodes/FileSystemEntry.h"

namespace CppImportLight {

// A CppParser converts c++ files to a list of nodes (Model::Node)
class CPPIMPORTLIGHT_API CppParser : public Interaction::COpenDir::Parser {
	public:
		virtual bool canParseFile(QString filename) override;
		virtual Model::FileSystemEntry* parseFile(QString filename) override;
};

}
