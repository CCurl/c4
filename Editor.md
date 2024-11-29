# The c4 editor

The c4 editor is an editor inspired by, and similar to, a stripped-down version of VI.

Enable the editor using `#define EDITOR` (editor.c)

## Modes
There are 4 modes in the editor:
- NORMAL
- INSERT
- REPLACE
- COMMAND

### These control keys are active in all modes:

| Key       | Action |
| :--       | :-- |
| [ctrl]+a  | Insert tag: COMPILE |
| [ctrl]+b  | Insert tag: DEFINE |
| [ctrl]+c  | Insert tag: INTERP |
| [ctrl]+d  | Insert tag: COMMENT |
| [ctrl]+h  | Left 1 char |
| [ctrl]+j  | Down 1 line |
| [ctrl]+k  | Up 1 line |
| [ctrl]+l  | Right 1 char |
| [tab]     | Right 8 chars |
| [ctrl]+i  | Right 8 chars |
| [ctrl]+x  | Delete the char under the cursor |
| [ctrl]+z  | Delete the char to the left of the cursor |
| [esc-esc] | Goto NORMAL mode |

### NORMAL mode

The movement keys are similar to those in VI:

| Key  | Action|
| :--  | :-- |
| h    | Left 1 char |
| j    | Down 1 line |
| k    | Up 1 line |
| l    | Right 1 char |
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
| #    | Redraw the screen |
| a    | Append: move right 1 char and change to INSERT mode |
| A    | Append: goto the end of the line and change to INSERT mode |
| b    | Insert a BLANK/SPACE into the current line |
| B    | Insert a BLANK/SPACE into the block |
| c    | Change: Delete the current char and change to INSERT mode (same as 'xi') |
| C    | Change: Delete to the end of the line and change to INSERT mode (same as 'd$A') |
| d.   | Delete the char under the cursor (same as 'x') |
| d$   | Delete to the end of the line |
| dd   | Copy the current line into the YANK buffer and delete the line |
| dw   | Delete to the end of the current word |
| D    | Delete to the end of the line (same as 'd$') |
| g    | Goto the top-left of the screen |
| G    | Goto the bottom-left of the screen |
| i    | Insert: change to INSERT mode |
| I    | Insert: goto the beginning of the line and change to INSERT mode |
| J    | Join the current and next lines together |
| o    | Insert an empty line BELOW the current line and change to INSERT mode |
| O    | Insert an empty line ABOVE the current line and change to INSERT mode |
| p    | Paste the YANK buffer into a new line BELOW the current line |
| P    | Paste the YANK buffer into a new line ABOVE the current line |
| r    | Replace the char under the cursor with the next key pressed (if printable) |
| R    | Change to REPLACE mode |
| x    | Delete the char under the cursor to the end of the line |
| X    | Delete the char under the cursor to the end of the block |
| Y    | Copy the current line into the YANK buffer |
| z    | Delete the char to the left of the cursor (same as 'hx') |

### INSERT mode

In INSERT mode, all printable characters are inserted into the block.

Carriage-Return inserts a new line.

### REPLACE mode

In REPLACE mode, all printable characters are placed into the block.

Carriage-Return moves to the beginning of the next line.

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
