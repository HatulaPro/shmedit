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
  {ACTION_PASTE_CLIPBOARD, {"PasteClipboard", &Content::actionPasteFromClipboard}},
  {ACTION_OPEN_NEXT_FILE, {"OpenNextFile", &Content::openNextFile}},
  {ACTION_OPEN_PREV_FILE, {"OpenPrevFile", &Content::openPreviousFile}},
  {ACTION_UNDO, {"Undo", &Content::actionUndo}},
};

const std::map<std::string, std::string(Content::*)(std::string)> Config::calledCommands = {
  {COMMAND_OPEN, &Content::commandOpen},
  {COMMAND_EXIT, &Content::commandExit},
  {COMMAND_RENAME_FILE, &Content::commandRenameFile},
  {COMMAND_SAVE_AND_EXIT, &Content::commandSaveAndExit},
  {COMMAND_FIND, &Content::commandFind},
  {COMMAND_FIND_AND_REPLACE, &Content::commandFindAndReplace},
  {COMMAND_OPEN_FILE_EXPLORER, &Content::commandOpenFileExplorer},
};

std::map<std::string, void (Content::*)()> Config::instantCommands = {
  {COMMAND_SAVE, &Content::actionSaveFile},
  {COMMAND_QUIT, &Content::actionQuit},
  {COMMAND_QUIT_AND_SAVE, &Content::actionQuitAndSave},
  {COMMAND_PASTE, &Content::actionPaste} ,
  {COMMAND_PASTE_CLIPBOARD,&Content::actionPasteFromClipboard},
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

std::map<int, VisualAction> Config::visualCommands = {
  {ACTION_COPY_SELECTION, {"VisualCopy", &Content::actionCopySelection}},
  {ACTION_PASTE_SELECTION, {"VisualPaste", &Content::actionPasteSelection}},
  {ACTION_REMOVE, {"VisualDelete", &Content::actionDeleteSelection}},
  {ACTION_REMOVE_SELECTION, {"VisualDelete", &Content::actionDeleteSelection}},
  {ACTION_REMOVE_SELECTION_ALT, {"VisualDelete", &Content::actionDeleteSelection}},
  {ACTION_TABIFY, {"VisualTabify", &Content::actionTabifySelection}},
  {ACTION_UNTABIFY, {"VisualUntabify", &Content::actionUntabifySelection}},
  {ACTION_MOVE_LEFT, {"VisualMoveLeft", &Content::actionLeftKeySelection}},
  {ACTION_MOVE_RIGHT, {"VisualMoveRight", &Content::actionRightKeySelection}},
  {ACTION_MOVE_UP, {"VisualMoveUp", &Content::actionUpKeySelection}},
  {ACTION_MOVE_DOWN, {"VisualMoveDown", &Content::actionDownKeySelection}},
  {ACTION_GOTO_END_LINE, {"VisualJumpToLineEnd", &Content::actionJumpToLineEndSelection}},
  {ACTION_GOTO_START_LINE, {"VisualJumpToLineStart", &Content::actionJumpToLineStartSelection}},
  {ACTION_MOVE_WORD_RIGHT, {"VisualMoveWordRight", &Content::actionWordRightSelection}},
  {ACTION_MOVE_WORD_LEFT, {"VisualMoveWordLeft", &Content::actionWordLeftSelection}},
  {ACTION_MOVE_LINE_UP, {"VisualMoveLineUp", &Content::actionMoveLineUpSelection}},
  {ACTION_MOVE_LINE_DOWN, {"VisualMoveLineDown", &Content::actionMoveLineDownSelection}},
  {ACTION_SELECT_LINES, {"VisualSelectLines", &Content::actionSelectLinesSelection}},
  {ACTION_PAGE_UP, {"VisualPageUp", &Content::actionPageUpSelection}},
  {ACTION_PAGE_DOWN, {"VisualPageDown", &Content::actionPageDownSelection}},
};

std::map<std::string, int> Config::settings = {
	{"USE_TABS", 1},
	{"TAB_SIZE", 4},
};


bool Config::parse(std::string fileName)
{
	std::stringstream s(FilesUtil::readFile(fileName).c_str());

	std::string line;
	size_t colonIndex;
	size_t equalsIndex;
	std::string funcName;
	std::string value;
	int key;
	while (std::getline(s, line)) {
		line = Helper::trim(line);
		if (!line.size()) continue;
		if (line[0] == '#') continue;


		colonIndex = line.find_first_of(':');
		equalsIndex = line.find_first_of('=');

		// Action reassignment (such as 'MoveLeft:^L')
		if (colonIndex != std::string::npos && colonIndex < line.size() - 1) {
			funcName = Helper::trim(line.substr(0, colonIndex));
			value = Helper::trim(line.substr(colonIndex + 1));

			if (!funcName.size() || !value.size()) continue;
			key = Config::getKeybind(value);
			if (!key) continue;

			// Handling oneClickActions:
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

			// Handling visualCommands:
			for (auto f = Config::visualCommands.begin(); f != Config::visualCommands.end(); f++) {
				if (f->second.first == funcName) {
					if (Config::visualCommands.find(key) == Config::visualCommands.end()) {
						Config::visualCommands[key] = f->second;
					}
					else {
						auto tmp = Config::visualCommands[key];
						Config::visualCommands[key] = f->second;
						Config::visualCommands[f->first] = tmp;
					}
				}
			}
		}
		// Setting shortcut for another instant command
		else if (equalsIndex != std::string::npos && equalsIndex < line.size() - 1) {
			funcName = Helper::trim(line.substr(0, equalsIndex));
			value = Helper::trim(line.substr(equalsIndex + 1));

			if (Config::instantCommands.find(value) != Config::instantCommands.end()) {
				if (funcName[0] < 'A' || funcName[0] > 'Z') continue;

				Config::instantCommands[funcName] = Config::instantCommands[value];
			}
		}
		return true;
	}
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
	else if (value == "@RIGHT") {
		return 40192;
	}
	else if (value == "@LEFT") {
		return 39680;
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
