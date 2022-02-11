#pragma once
#include "Content.h"
#include <map>
#include <string>

// A pair of string name and a oneClickAction function.
typedef std::pair<std::string, void (Content::*)()> OneClickAction;
typedef std::pair<std::string, void (Content::*)()> VisualAction;

class Config {
public:
	static bool parse(std::string fileName);
	static int getKeybind(std::string value);

	static std::map<int, OneClickAction> oneClickActions;
	static std::map<int, VisualAction> visualCommands;

	static const std::map<std::string, void(Content::*)()> instantCommands;
	static const std::map<std::string, std::string(Content::*)(std::string)> calledCommands;
};