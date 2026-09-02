# pmenu

A dmenu for [sword](https://github.com/oscarpavon/sword).

pmenu filters a list and prints what was chosen. It reads items from stdin one
per line, shows them in a strip across the top of the screen, narrows them as
you type, and prints the selected one to stdout. It does not know what the
items are and does not run anything - `pmenu_run` is the shell script that
feeds it `$PATH` and runs what comes back.

```sh
ls /usr/bin | pmenu          # pick a name, get it on stdout
pmenu_run                    # the launcher
```

## Keys

| key | |
| --- | --- |
| type | narrow the list |
| left / right, tab | move the selection |
| backspace | delete a character |
| ctrl+u | clear what was typed |
| enter | print the selection and exit |
| escape | print nothing and exit |

Losing the keyboard closes it too: a menu nobody can type into has nothing left
to do.

Enter with nothing matching prints what was typed, so a command that is not in
the list can still be run.

## How it is on top

There is no `wlr-layer-shell` in sword, so pmenu is an ordinary xdg-shell
window. What puts it across the top of the output is that it calls itself
`pmenu` with `xdg_toplevel.set_app_id`, and sword's `layout_place_launcher()`
floats any window with that app id into a full width strip and raises it. On
any other compositor pmenu is a small ordinary window.

## Build

```sh
make
make install     # /usr/bin/pmenu and /usr/bin/pmenu_run
```

Needs [pway](https://github.com/oscarpavon/pway) for the window and the shm
buffers, [pfonts](https://github.com/oscarpavon/pfonts) for the text, and
pixman, which is what pfonts' CPU backend composites glyphs with. No GL context
is created and no GPU is used - the menu is drawn into a wl_shm buffer on the
CPU.

The font is in `config.h`, along with the colours and the prompt.
