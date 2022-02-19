#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <cassert>

using namespace std;

// Number of lines to skip at the beginning of an export file.
const int EXPORT_FILE_HEADER_SIZE = 19;

// This is the size of the prefix of the line which we do not care about.
// Lines should be at least as long as this size.
const int SYMBOL_EXPORT_LINE_PREFIX_LENGTH = 26;

void readLines(char* filename, list<string>& lines, bool ignore_equal_symbols_at_end)
{
	cout << "Opening file " << filename << "    ";
	ifstream in(filename);
	assert(in);
	
	string line;
	int lineNum = 0;
	while (getline(in, line))
	{	
		++lineNum;
		
		if (lineNum <= EXPORT_FILE_HEADER_SIZE) continue;
		if (line.length() == 0) break;
		assert(line.length() > SYMBOL_EXPORT_LINE_PREFIX_LENGTH);
		
		line = line.substr(SYMBOL_EXPORT_LINE_PREFIX_LENGTH,
			line.length() - SYMBOL_EXPORT_LINE_PREFIX_LENGTH);
		if (!ignore_equal_symbols_at_end)lines.push_back(line);
		else lines.push_back(line.substr(0, line.find(" = ")));	
	}
	cout << " found " << lines.size() << " lines" << Qt::endl;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "This tool is used to extract the difference in exported symbols between two dlls." << Qt::endl;
		cout << "You need to provide two text files as input." << Qt::endl;
		cout << "Each file is the output of 'dumpbin /exports mydll.dll'" << Qt::endl;
		return 0;
	}
	

	list<string> lines1;
	readLines(argv[1], lines1, true);
	list<string> lines2;
	readLines(argv[2], lines2, true);

	cout << endl << "Lines only in " << argv[1] << Qt::endl;
	int identicalLines = 0;
	for(auto line : lines1)
	{
		bool found{};
		
		auto it2 = lines2.begin();
		while ( it2 != lines2.end() )
		{
			if (line == *it2)
			{
				++identicalLines;
				lines2.erase(it2); // Delete the line since we determined it is a duplicate
				found = true;
				break;
			}
			++it2;
		}			
			
		if (!found) cout << line << Qt::endl;
	}
	
	cout << endl << "Lines only in " << argv[2] << Qt::endl;
	for (auto l : lines2) cout << l << Qt::endl;
	cout << endl << identicalLines << " identical lines" << Qt::endl;
	return 0;
}