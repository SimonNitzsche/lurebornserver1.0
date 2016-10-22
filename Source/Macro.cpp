#include "Macro.h"

#include <stdlib.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Logger.h"
#include "ChatCommands.h"
#include "Social.h"
#include "dirent.h"

using namespace std;

map<string, vector<string>> Macro::macros;

std::string getPercentString(const string& msg, int perc) {
	std::stringstream ss;
	ss << "\r" << Logger::getLogStream("MCRO", "LOAD").str() << msg;
	ss << " " << to_string(perc) << "%" << flush;
	return ss.str();
}

double getPercentOf(double cur, double max) {
	if (max>0) {
		return (100.0 / (double)max)*(double)cur;
	}
	
	return -1;
}

int countDir() {
	DIR *dp;
	int i=0;
	struct dirent *ep;
	dp = opendir("./../Env/macros/");

	if (dp != NULL)
	{
		while (ep = readdir(dp))
			i++;

		(void)closedir(dp);
	}
	return i;
}

void Macro::ExecuteMacro(string macroFile, SessionInfo *s) {
	try {
		std::vector<string> scl = Macro::macros.at(macroFile);
		if (scl.size() > 0)
			for each (string sc in scl)
				ChatCommandManager::handleCommand(std::wstring(sc.begin(), sc.end()), s);
	}catch(const exception&){}
}

void Macro::EchoMacros(long long charId) {
	for (auto iterator = Macro::macros.begin(); iterator != Macro::macros.end(); iterator++)
		Chat::sendChatMessage(charId, wstring(iterator->first.begin(),iterator->first.end()));
}

void Macro::LoadMacros() {
	macros.clear();

	DIR *dir;
	struct dirent *ent;
	int lProg = 0;
	int ct = countDir();
	int it = 0;
	std::string fs = "Load macros...";
	cout << getPercentString(fs, 0);
	if ((dir = opendir("./../Env/macros/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			it++;
			std::string file = ent->d_name;
			if (file.size() > 3) {
				if (file.substr(file.size() - 3, 3) == "scm") {
					std::ifstream infile("./../Env/macros/" + file);
					std::string line;
					std::vector<std::string> mcommands;
					mcommands.clear();
					while (std::getline(infile, line)) {
						if (line.substr(0, 1) == "/")
							mcommands.push_back(line);
					}
					macros.insert(macros.begin(), std::pair<std::string,std::vector<std::string>>(file,mcommands));
					int nProg = getPercentOf(it, ct);
					if (nProg > lProg) {
						cout << getPercentString(fs, nProg);
						lProg = nProg;
					}
				}
			}
		}
		cout << getPercentString(fs, 100) << "\n";
		closedir(dir);
	}
	else {
		Logger::logError("MACRO", "LOAD", "LIST", "Cannot open directory.");
	}

}