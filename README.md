# tuigo

a terminal go / weiqi engine written in c using ncurses.

## features

- human vs human and human vs ai game modes
- play as black or white
- unicode board and stones rendered via ncursesw
- mcts (monte carlo tree search) ai engine with ucb1 selection
- four difficulty presets (easy, normal, hard, very hard)
- japanese scoring with komi
- ko rule enforcement, suicide prevention
- undo support
- scales to terminal size, supports 9x9, 13x13, and 19x19 boards

## controls

- arrows / hjkl: move cursor
- enter: place stone
- p: pass
- u: undo
- q: return to menu

## building

**debian / ubuntu:**
```
sudo apt install libncursesw5-dev
make
```

**arch / manjaro:**
```
sudo pacman -S ncurses
make
```

**fedora / rhel:**
```
sudo dnf install ncurses-devel
make
```

**windows (msys2 mingw64):**
```
pacman -S mingw-w64-x86_64-ncurses
make
```

## running

```
./tuigo
```
