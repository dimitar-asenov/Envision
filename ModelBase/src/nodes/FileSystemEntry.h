#pragma once

#include "../modelbase_api.h"

#include "Text.h"
#include "TypedList.h"
#include "List.h"
#include "composite/CompositeNode.h"
#include "nodeMacros.h"

#include "Core/src/reflect/Reflect.h"

namespace Model {
	class FileSystemEntry;
}
extern template class MODELBASE_API Model::TypedList<Model::FileSystemEntry>;

namespace Model {

class MODELBASE_API FileSystemEntry : public Super<CompositeNode>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(FileSystemEntry)
	ATTRIBUTE_VALUE_CUSTOM_RETURN(Text, name, setName, QString, const QString&)
	ATTRIBUTE(TypedList<FileSystemEntry>, subEntries, setSubEntries)
	ATTRIBUTE(List, content, setContent)

	public:
		FileSystemEntry(const QString& name);
};

}
