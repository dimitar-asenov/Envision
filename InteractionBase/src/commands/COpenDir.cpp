#include "COpenDir.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/TextRenderer.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/views/MainView.h"
#include "ModelBase/src/nodes/FileSystemEntry.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/model/TreeManager.h"

using namespace Visualization;

namespace Interaction {

using Model::FileSystemEntry;
using Model::Text;

// A text parser just converts any file to a list of lines (Model::Text)
class PlainTextParser : public COpenDir::Parser {
	public:
		static constexpr const int MaxTextSize = 100000;
		virtual bool canParseFile(QString filename) override {return QFile{filename}.size() <= MaxTextSize;}
		virtual FileSystemEntry* parseFile(QString filename) override {
			QFile file{filename};
			Q_ASSERT(file.exists());

			auto result = new FileSystemEntry{QFileInfo{filename}.fileName()};
			if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
				QTextStream textStream(&file);
				while (true)
				{
					QString line = textStream.readLine();
					if (line.isNull()) break;

					result->content()->append(new Text{line});
				}
			}
			return result;
		}
};

std::vector<std::unique_ptr<COpenDir::Parser>>& COpenDir::parsers() {
	static std::vector<std::unique_ptr<Parser>> parsers;
	return parsers;
}

void COpenDir::registerFileParser(std::unique_ptr<Parser> parser) {
	parsers().emplace_back(std::move(parser));
}

COpenDir::Parser::~Parser() {}; // Emit vtable here.


FileSystemEntry* COpenDir::openDir(QString directoryPath) {
	auto result = new FileSystemEntry{QFileInfo{directoryPath}.fileName()};

	QDir dir{directoryPath};
	for (auto entry : dir.entryInfoList(QDir::AllEntries | QDir::NoDot | QDir::NoDotDot)) {
		if (entry.isDir()) {
			result->subEntries()->append(openDir(entry.absoluteFilePath()));
		} else {
			result->subEntries()->append(parseFile(entry.absoluteFilePath()));
		}
	}
	return result;
}


FileSystemEntry* COpenDir::parseFile(QString filePath) {
	for (const auto& parser : parsers()) {
		if (parser->canParseFile(filePath)) {
			return parser->parseFile(filePath);
		}
	}

	// Try the text parser, if nothing else worked.
	PlainTextParser parser;
	if (parser.canParseFile(filePath))
		return parser.parseFile(filePath);

	return new FileSystemEntry{QFileInfo{filePath}.fileName()};
}


COpenDir::COpenDir() : Command{"opendir"}{}

bool COpenDir::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens,
		const std::unique_ptr<Visualization::Cursor>&)
{
	return (commandTokens.size() == 1 || commandTokens.size() == 2) && commandTokens.first() == "opendir";
}

CommandResult* COpenDir::execute(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens,
		const std::unique_ptr<Visualization::Cursor>&)
{
	if (commandTokens.size() < 2) return new CommandResult{new CommandError{"Please specify a directory to open"}};

	QString dirName = commandTokens[1];
	if (dirName.startsWith("\"") || dirName.startsWith("'")) dirName = dirName.mid(1, dirName.length()-2);

	if (!QDir{dirName}.exists()) {
		return new CommandResult{new CommandError{"Directory " + dirName + "does not exist"}};
	}

	auto manager = new Model::TreeManager{openDir(dirName)};

	auto mainScene = VisualizationManager::instance().mainScene();
	mainScene->addTopLevelNode(manager->root());
	mainScene->listenToTreeManager(manager);
	mainScene->setApproximateUpdate(true);

	Visualization::MainView::centerAndZoomViewToFitEntireScene();

	return new CommandResult{};
}

QList<CommandSuggestion*> COpenDir::suggest(Visualization::Item*, Visualization::Item*, const QString& textSoFar,
		const std::unique_ptr<Visualization::Cursor>&)
{
	QList<CommandSuggestion*> s;

	if (textSoFar.trimmed().startsWith("opendir ", Qt::CaseInsensitive))
	{
		auto searchText = textSoFar.trimmed().mid(QString{"opendir"}.length());
		s.append(new CommandSuggestion{"opendir " + searchText, "Open directory " + searchText});
	}
	else if (QString{"opendir "}.startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
			s.append(new CommandSuggestion{"opendir ", "Open directory"});

	return s;
}

}
