# The c4 editor

The c4 editor is somewhat similar to a stripped-down version of VI. <br/>
Enable the editor using `#define EDITOR` (in c4.h) <br/>
The current block being edited is in 'block@'. <br/>
Use '23 edit' to edit block #23. <br/>
Use 'ed' to re-edit the last edited block. </br>
While in the editor, use ':!5 block!' to switch to another block. <br/>

## Modes
There are 4 modes in the editor:
- NORMAL
- INSERT
- REPLACE
- COMMAND

### These control keys are active in all modes:

| Key         | Action |
| :--         | :-- |
| [ctrl]+a    | Insert/replace tag: COMPILE |
| [ctrl]+b    | Insert/replace tag: DEFINE |
| [ctrl]+c    | Insert/replace tag: INTERP |
| [ctrl]+d    | Insert/replace tag: COMMENT |
| [ctrl]+e    | Send the current line to the outer interpreter |
| [ctrl]+h    | Left 1 char (and delete it if in INSERT mode) |
| [ctrl]+i    | Right 8 chars (also [tab]) |
| [shift]+tab | Left 8 chars |
| [ctrl]+j    | Down 1 line |
| [ctrl]+k    | Up 1 line |
| [ctrl]+l    | Right 1 char |
| [ctrl]+q    | Left 8 chars |
| [ctrl]+s    | Save the block to disk |
| [ctrl]+Home | Goto the beginning of the first line |
| [ctrl]+End  | Goto the beginning of the last line |
| [ctrl]+x    | Delete the char under the cursor |
| [esc-esc]   | Goto NORMAL mode |

### NORMAL mode

| Key  | Action|
| :--  | :-- |
| $    | Goto the end of the line |
| _    | Goto the beginning of the line |
| [CR] | Goto the beginning of the next line |
| [SP] | Right 1 char |
| 1    | Set tag: COMPILE |
| 2    | Set tag: DEFINE |
| 3    | Set tag: INTERP |
| 4    | Set tag: COMMENT |
| :    | Change to COMMAND mode |
| +    | Save the current block and goto the next block |
| -    | Save the current block and goto the previous block |
| /    | Specify search string, jump to next occurrence |
| #    | Redraw the screen |
| a    | Append: move right 1 char and change to INSERT mode (same as 'li') |
| A    | Append: goto the end of the line and change to INSERT mode  (same as '$i') |
| b    | Insert a BLANK/SPACE into the current line |
| B    | Insert a BLANK/SPACE to the end of the block |
| c    | Change: Delete the current char and change to INSERT mode (same as 'xi') |
| C    | Change: Delete to the end of the line and change to INSERT mode (same as 'd$i') |
| d.   | Delete the char under the cursor (same as 'x') |
| d$   | Delete to the end of the line |
| dd   | Copy the current line into the YANK buffer and delete the line |
| dw   | Delete the current word |
| D    | Delete to the end of the line (same as 'd$') |
| g    | Goto the top-left of the screen |
| G    | Goto the bottom-left of the screen |
| h    | Left 1 char |
| i    | Change to INSERT mode |
| I    | Goto the beginning of the line and change to INSERT mode (same as '_i') |
| j    | Down 1 line |
| J    | Join the current and next lines together |
| k    | Up 1 line |
| l    | Right 1 char |
| m    | Down 4 lines |
| M    | Up 4 lines |
| n    | Jump to next occurrence of search string |
| N    | Jump to previous occurrence of search string |
| o    | Insert an empty line BELOW the current line and change to INSERT mode |
| O    | Insert an empty line ABOVE the current line and change to INSERT mode |
| p    | Paste the YANK buffer into a new line BELOW the current line |
| P    | Paste the YANK buffer into a new line ABOVE the current line |
| q    | Right 4 chars |
| Q    | Left 4 chars |
| r    | Replace the char under the cursor with the next key pressed (if printable) |
| R    | Change to REPLACE mode |
| S    | Switch to last edited block |
| w    | Move right to the beginning of the next word |
| W    | Move left to the beginning of the previous word |
| x    | Delete the char under the cursor to the end of the line |
| X    | Delete the char to the left of the cursor (same as 'hx') |
| Y    | Copy the current line into the YANK buffer |
| Z    | Delete the char under the cursor to the end of the block |

### INSERT mode

In INSERT mode, all printable characters are inserted into the line.
- [Enter] inserts a new line at the cursor.
- [Backspace] moves left and deletes that character.

### REPLACE mode

In REPLACE mode, all printable characters are placed into the line.
- [Enter] moves to the beginning of the next line.
- [Backspace] moves left and does not delete that character.

### COMMAND mode

COMMAND mode is invoked when pressing ':' in NORMAL mode.

| Command | Action|
| :--     | :-- |
| rl      | ReLoad: discard all changes and reload the current block |
| w       | Write the current block if it has changed |
| w!      | Write the current block, even if it has NOT changed |
| wq      | Write the current block and quit (same as ':w' ':q') |
| q       | Quit, if the current block has NOT changed |
| q!      | Quit, even if the current block has changed |
| ![any]  | Send [any] to the c4 outer interpreter |
