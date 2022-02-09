#include "Config.h"
#include "helpers/FilesUtil.h"
#include "helpers/Helper.h"
#include <sstream>
#include <iostream>

std::map<int, OneClickAction> Config::oneClickActions = {
	{ACTION_NEWLINE, {"Newline", &Content::actionEnter}},
	{ACTION_INSTANT_NEWLINE, {"InstantNewline", &Content::actionEnterNewline}},
	{ACTION_REMOVE, {"Remove", &Content::actionRemove}},
	{ACTION_REMOVE_WORD, {"RemoveWord", &Content::actionRemoveWord}},
	{ACTION_SAVE, {"SaveFile", &Content::actionSaveFile}},
	{ACTION_DUPLICATE_LINE, {"DuplicateLine", &Content::actionDuplicateLine}},
	{ACTION_DELETE_LINE, {"DeleteLine", &Content::actionDeleteLine}},
	{ACTION_TABIFY, {"Tabify", &Content::actionTabify}},
	{ACTION_UNTABIFY, {"Untabify", &Content::actionUntabify}},
	{ACTION_DELETE, {"Delete", &Content::actionDelete}},
	{ACTION_DELETE_WORD, {"DeleteWord", &Content::actionDeleteWord}},
	{ACTION_MOVE_LEFT, {"MoveLeft", &Content::actionLeftKey}},
	{ACTION_MOVE_RIGHT, {"MoveRight", &Content::actionRightKey}},
	{ACTION_MOVE_UP, {"MoveUp", &Content::actionUpKey}},
	{ACTION_MOVE_DOWN, {"MoveDown", &Content::actionDownKey}},
	{ACTION_MOVE_WORD_RIGHT, {"MoveWordRight", &Content::actionWordRight}},
	{ACTION_MOVE_WORD_LEFT, {"MoveWordLeft", &Content::actionWordLeft}},
	{ACTION_MOVE_LINE_UP, {"MoveLineUp", &Content::actionMoveLineUp}},
	{ACTION_MOVE_LINE_DOWN, {"MoveLineDown", &Content::actionMoveLineDown}},
	{ACTION_GOTO_END_LINE, {"JumpToLineEnd", &Content::actionJumpToLineEnd}},
	{ACTION_GOTO_START_LINE, {"JumpToLineStart", &Content::actionJumpToLineStart}},
	{ACTION_PAGE_UP, {"PageUp", &Content::actionPageUp}},
	{ACTION_PAGE_DOWN, {"PageDown", &Content::actionPageDown}},
};

const std::map<std::string, std::string(Content::*)(std::string, int&, int&)> Config::calledCommands = {
	{COMMAND_OPEN, &Content::commandOpen},
	{COMMAND_FIND, &Content::commandFind},
	{COMMAND_FIND_AND_REPLACE, &Content::commandFindAndReplace},
};

const std::map<std::string, void (Content::*)(int&, int&)> Config::instantCommands = {
	{COMMAND_SAVE, &Content::actionSaveFile},
	{COMMAND_QUIT, &Content::actionQuit},
	{COMMAND_QUIT_AND_SAVE, &Content::actionQuitAndSave},
	{COMMAND_PASTE, &Content::actionPaste},
	{COMMAND_DELETE_WORD, &Content::actionDeleteWord},
	{COMMAND_REMOVE_WORD, &Content::actionRemoveWord},
	{COMMAND_DELETE_LINE, &Content::actionDeleteLine},
	{COMMAND_MOVE_WORD, &Content::actionWordRight},
	{COMMAND_BACK_WORD, &Content::actionWordLeft},
	{COMMAND_TABIFY, &Content::actionTabify},
	{COMMAND_UNTABIFY, &Content::actionUntabify},
	{COMMAND_MOVE_LEFT, &Content::actionLeftKey},
	{COMMAND_MOVE_RIGHT, &Content::actionRightKey},
	{COMMAND_MOVE_UP, &Content::actionUpKey},
	{COMMAND_MOVE_DOWN, &Content::actionDownKey},
	{COMMAND_COPY_WORD, &Content::actionCopyWord},
	{COMMAND_COPY_LINE, &Content::actionCopyLine},
	{COMMAND_COPY_WORD_BACK, &Content::actionCopyWordBack},
};

const std::map<int, void (Content::*)(int&, int&, int&, int&)> Config::visualCommands = {
	{ACTION_COPY_SELECTION, &Content::actionCopySelection},
	{ACTION_PASTE_SELECTION, &Content::actionPasteSelection},
	{ACTION_REMOVE, &Content::actionDeleteSelection},
	{ACTION_REMOVE_SELECTION, &Content::actionDeleteSelection},
	{ACTION_REMOVE_SELECTION_ALT, &Content::actionDeleteSelection},
	{ACTION_TABIFY, &Content::actionTabifySelection},
	{ACTION_UNTABIFY, &Content::actionUntabifySelection},
	{ACTION_MOVE_LEFT, &Content::actionLeftKeySelection},
	{ACTION_MOVE_RIGHT, &Content::actionRightKeySelection},
	{ACTION_MOVE_UP, &Content::actionUpKeySelection},
	{ACTION_MOVE_DOWN, &Content::actionDownKeySelection},
	{ACTION_GOTO_END_LINE, &Content::actionJumpToLineEndSelection},
	{ACTION_GOTO_START_LINE, &Content::actionJumpToLineStartSelection},
	{ACTION_MOVE_WORD_RIGHT, &Content::actionWordRightSelection},
	{ACTION_MOVE_WORD_LEFT, &Content::actionWordLeftSelection},
	{ACTION_MOVE_LINE_UP, &Content::actionMoveLineUpSelection},
	{ACTION_MOVE_LINE_DOWN, &Content::actionMoveLineDownSelection},
	{ACTION_SELECT_LINES, &Content::actionSelectLinesSelection},
	{ACTION_PAGE_UP, &Content::actionPageUpSelection},
	{ACTION_PAGE_DOWN, &Content::actionPageDownSelection},
};

bool Config::parse(std::string fileName)
{
	std::stringstream s(FilesUtil::readFile(fileName).c_str());

	std::string line;
	size_t colonIndex;
	std::string funcName;
	std::string value;
	int key;
	while (std::getline(s, line)) {
		line = Helper::trim(line);
		if (!line.size()) continue;
		if (line[0] == '#') continue;
		colonIndex = line.find_first_of(':');
		if (colonIndex == std::string::npos && colonIndex < line.size() - 1) continue;

		funcName = Helper::trim(line.substr(0, colonIndex));
		value = Helper::trim(line.substr(colonIndex + 1));

		if (!funcName.size() || !value.size()) continue;
		key = Config::getKeybind(value);
		if (!key) continue;

		for (auto f = Config::oneClickActions.begin(); f != Config::oneClickActions.end(); f++) {
			if (f->second.first == funcName) {
				if (Config::oneClickActions.find(key) == Config::oneClickActions.end()) {
					Config::oneClickActions[key] = f->second;
				}
				else {
					auto tmp = Config::oneClickActions[key];
					Config::oneClickActions[key] = f->second;
					Config::oneClickActions[f->first] = tmp;
				}
			}
		}
	}

	return true;
}

int Config::getKeybind(std::string value)
{
	if (value == "Enter") {
		return '\r';
	}
	else if (value == "^Enter") {
		return '\n';
	}
	else if (value == "Tab") {
		return '\t';
	}
	else if (value == "^Tab") {
		return 37888;
	}
	else if (value == "Del") {
		return 21216;
	} 
	else if (value == "^Del") {
		return 37600;
	}
	else if (value == "LEFT") {
		return 19168;
	}
	else if (value == "RIGHT") {
		return 19680;
	}
	else if (value == "UP") {
		return 18400;
	}
	else if (value == "DOWN") {
		return 20448;
	}
	else if (value == "^RIGHT") {
		return 29664;
	}
	else if (value == "^LEFT") {
		return 29408;
	}
	else if (value == "@UP") {
		return 38912;
	}
	else if (value == "@DOWN") {
		return 40960;
	}
	else if (value == "Rem") {
		return 8;
	}
	else if (value == "^Rem") {
		return 127;
	}
	else if (value == "*LEFT") {
		return 18144;
	}
	else if (value == "*RIGHT") {
		return 20192;
	}
	else if (value == "*UP") {
		return 18656;
	}
	else if (value == "*DOWN") {
		return 20704;
	}
	else if (value == "^]") {
		return 29;
	}
	else if (value == "^\\") {
		return 28;
	}
	else if (value == "^[") {
		return 27;
	}
	else if (value == "*]" || value == "@]") {
		return 93;
	}
	else if (value == "*\\" || value == "@\\") {
		return 92;
	}
	else if (value == "*[" || value == "@[") {
		return 91;
	}
	// In case of Ctrl + letter
	if (value.size() == 2 && value[0] == '^' && value[1] >= 'A' && value[1] <= 'Z') {
		return value[1] - 'A' + 1;
	}
	return 0;
}
