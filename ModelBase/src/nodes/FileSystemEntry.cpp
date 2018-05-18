#include "FileSystemEntry.h"

#include "TypedList.hpp"

template class Model::TypedList<Model::FileSystemEntry>;

namespace Model {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(FileSystemEntry)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(FileSystemEntry)

DEFINE_ATTRIBUTE(FileSystemEntry, name, Text, false, false, true)
DEFINE_ATTRIBUTE(FileSystemEntry, subEntries, TypedListOfFileSystemEntry, false, false, true)
DEFINE_ATTRIBUTE(FileSystemEntry, content, List, false, false, true)

FileSystemEntry::FileSystemEntry(const QString& name) : Super{nullptr, FileSystemEntry::getMetaData()}
{
	setName(name);
}

}
