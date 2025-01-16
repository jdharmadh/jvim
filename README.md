# jvim

A memory-safe and efficient Vim clone written in C. Supports some vim keybindings and find/replace so far.

## Installation

Clone the repository and run `make`.

## Usage

`./jvim [filename]`

Use an existing filename to load a file, or otherwise to create a new file.

## Keybindings

- `i` for insert mode
- `0` for beginning of line
- `$` for end of line
- `G` for end of file
- `o` for new line below
- `O` for new line above
- `n` to go to next result in find/replace mode
- `[Enter]` to replace in find/replace mode

## Commands

- `:wq` to save and quit
- `:q!` to quit
- `:find [str]` to match all instances of `str`
- `:replace [str1] [str2]` to open find/replace

## Reach out

Reach out with any ideas!