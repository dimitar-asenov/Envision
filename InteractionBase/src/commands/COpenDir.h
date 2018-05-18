#pragma once

#include "../interactionbase_api.h"

#include "Command.h"

namespace Model {
	class FileSystemEntry;
}

namespace Interaction {

class INTERACTIONBASE_API COpenDir : public Command
{
	public:
		class Parser {
			public:
				virtual ~Parser();
				virtual bool canParseFile(QString filename) = 0;
				virtual Model::FileSystemEntry* parseFile(QString filename) = 0;
		};

		COpenDir();

		virtual bool canInterpret(Visualization::Item* source, Visualization::Item* target,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor) override;
		virtual CommandResult* execute(Visualization::Item* source, Visualization::Item* target,
				const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>& cursor) override;

		virtual QList<CommandSuggestion*> suggest(Visualization::Item* source, Visualization::Item* target,
				const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>& cursor) override;

		// Registers the parser and retains ownership.
		static void registerFileParser(std::unique_ptr<Parser> parser);

	private:
		static std::vector<std::unique_ptr<Parser>>& parsers();
		Model::FileSystemEntry* openDir(QString directoryPath);
		Model::FileSystemEntry* parseFile(QString filePath);
};

}
