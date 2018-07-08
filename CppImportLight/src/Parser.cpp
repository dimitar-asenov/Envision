#include "Parser.h"

#include "ModelBase/src/nodes/FileSystemEntry.h"
#include "ModelBase/src/nodes/Text.h"

namespace CppImportLight {

using Model::FileSystemEntry;
using Model::Text;

bool CppParser::canParseFile(QString filename) {
	return filename.endsWith(".h") || filename.endsWith(".hpp") ||
			 filename.endsWith(".cc") || filename.endsWith(".cpp");
}

Model::FileSystemEntry* CppParser::parseFile(QString filename) {
	QFile file(filename);
	bool openOk = file.open(QIODevice::ReadOnly);
	Q_ASSERT(openOk);
	auto bytes = file.readAll();

	clang::SourceManagerForFile managerForfile{filename.toStdString(), clang::StringRef(bytes.data(), bytes.size())};
	auto& sourceManager = managerForfile.get();
	auto mainFileID = sourceManager.getMainFileID();

	clang::LangOptions langOptions;
	langOptions.CPlusPlus = 1;
	langOptions.CPlusPlus11 = 1;
	langOptions.CPlusPlus14 = 1;
	langOptions.CPlusPlus17 = 1;
	langOptions.CPlusPlus2a = 1;
	langOptions.LineComment = 1;
	langOptions.CXXOperatorNames = 1;
	langOptions.Bool = 1;
	langOptions.ObjC1 = 1;
	langOptions.ObjC2 = 1;
	langOptions.MicrosoftExt = 1;
	langOptions.DeclSpecKeyword = 1;
	langOptions.CommentOpts.ParseAllComments = true;

	clang::Lexer lex{mainFileID, sourceManager.getBuffer(mainFileID), sourceManager, langOptions};
	lex.SetKeepWhitespaceMode(true);

	auto result = new FileSystemEntry{QFileInfo{filename}.fileName()};
	clang::Token tok;
	while (lex.LexFromRawLexer(tok) == false) {
		auto text = clang::StringRef(sourceManager.getCharacterData(tok.getLocation()), tok.getLength());
		result->content()->append(new Text{tok.getName() + QString::fromStdString(": " + text.str())});
	}

	return result;
}
}
