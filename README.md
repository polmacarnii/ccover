# CCover
Display the cover of the song you're listening on your terminal!
![demo](assets/demo.png) 
> [!NOTE]
> It Only Supports Kitty and CMUS at the moment

> [!NOTE]
> Not too stable at the moment, expect bugs!

## lyrics.sh 
![lyrics](assets/lyrics.png)
This is a Script made for displaying lyrics of the song you're listening
> [!NOTE]
> The lyrics.sh script **only works if there's a .lrc file**

# How to install
```
git clone https://github.com/polmacarnii/ccover.git
cd ccover
make
doas sh install.sh
```
> [!NOTE]
> If you use *sudo* instead of *doas*, change it

# Dependencies
## Build Dependencies
- LIBC (With Support for POSIX)
- A Compiler
- GNU Make
## Runtime Dependencies
- ffmpeg
- CMUS running
- A Kitty Terminal executing this program
- A Nerd Font installed

## Runtime dependencies for lyrics.
- sh 
- basic POSIX utilities
- gawk

# Compatibility
- Linux (x86_64 & aarch64) -> Working
- MacOS (aarch64) -> Working
However, if you system has all the Dependencies the most probable thing is that it would work
