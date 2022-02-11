#pragma once
#define ACTION_DELETE 21216
#define ACTION_DELETE_WORD 37600
#define ACTION_MOVE_LEFT 19168
#define ACTION_MOVE_RIGHT 19680
#define ACTION_MOVE_UP 18400
#define ACTION_MOVE_DOWN 20448
#define ACTION_MOVE_WORD_RIGHT 29664
#define ACTION_MOVE_WORD_LEFT 29408
#define ACTION_MOVE_LINE_UP 38912
#define ACTION_MOVE_LINE_DOWN 40960
#define ACTION_SAVE 19
#define ACTION_DUPLICATE_LINE 4
#define ACTION_DELETE_LINE 12
#define ACTION_NEWLINE '\r'
#define ACTION_INSTANT_NEWLINE '\n'
#define ACTION_REMOVE 8
#define ACTION_REMOVE_WORD 127
#define ACTION_GOTO_END_LINE 20192
#define ACTION_GOTO_START_LINE 18144
#define ACTION_START_COMMAND 3
#define ACTION_START_DEAFULT 21
#define ACTION_START_FIND 6
#define ACTION_START_FIND_AND_REPLACE 18
#define ACTION_START_VISUAL 2
#define ACTION_START_OPEN 15
#define ACTION_TABIFY 29
#define ACTION_UNTABIFY 27
#define ACTION_PAGE_UP 18656
#define ACTION_PAGE_DOWN 20704
#define ACTION_PASTE_CLIPBOARD 22

#define PAGE_UP_DOWN_SIZE 25

#define NEXT_IS_UTILS -32

#define BEGIN_CALLED_COMMAND ':'

#define COMMAND_SAVE "s"
#define COMMAND_OPEN "o"
#define COMMAND_FIND "f"
#define COMMAND_FIND_AND_REPLACE "fr"
#define COMMAND_QUIT "Q"
#define COMMAND_QUIT_AND_SAVE "q"
#define COMMAND_PASTE "p"
#define COMMAND_PASTE_CLIPBOARD "P"
#define COMMAND_DELETE_WORD "dw"
#define COMMAND_DELETE_LINE "dl"
#define COMMAND_REMOVE_WORD "db"
#define COMMAND_MOVE_WORD "w"
#define COMMAND_BACK_WORD "b"
#define COMMAND_TABIFY "ta"
#define COMMAND_UNTABIFY "ut"
#define COMMAND_MOVE_LEFT "h"
#define COMMAND_MOVE_RIGHT "l"
#define COMMAND_MOVE_UP "k"
#define COMMAND_MOVE_DOWN "j"
#define COMMAND_COPY_WORD "cw"
#define COMMAND_COPY_LINE "cl"
#define COMMAND_COPY_WORD_BACK "cb"

#define FIND_NEXT 'n'
#define FIND_AND_REPLACE_SKIP 's'

#define ACTION_COPY_SELECTION 'c'
#define ACTION_PASTE_SELECTION 'p'
#define ACTION_REMOVE_SELECTION 'r'
#define ACTION_REMOVE_SELECTION_ALT 'd'

#define ACTION_SELECT_LINES 12

#define EXIT_CMD_MODE 'u'