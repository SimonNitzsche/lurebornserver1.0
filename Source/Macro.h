#pragma once

#include <stdlib.h>
#include <map>
#include <string>
#include <vector>

#include "ChatCommands.h"

class Macro {
	public:
		static std::map<std::string, std::vector<std::string>> macros;
		static void ExecuteMacro(std::string macroFile, SessionInfo * s);
		static void EchoMacros(long long charId);
		static void LoadMacros();
};