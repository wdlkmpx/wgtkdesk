WGtkDesk
========

A collection of small apps for the desktop

## Forked apps

- asunder   [GPL2]  http://littlesvr.ca/asunder/releases/asunder-2.9.7.tar.bz2
- gdmap     [GPL2]  https://gitlab.com/sjohannes/gdmap
- gmrun     [MIT]   https://github.com/wdlkmpx/gmrun
- gpicview  [GPL2]  https://github.com/lxde/gpicview
- grsync    [GPL2]  https://sourceforge.net/projects/grsync/files/grsync-1.3.0.tar.gz
- gxmessage [GPL3]  https://trmusson.dreamhosters.com/stuff/gxmessage-3.4.3.tar.gz
- isomaster [GPL2]  http://littlesvr.ca/isomaster/releases/isomaster-1.3.16.tar.bz2
- lxcolor   [GPL2]  https://sourceforge.net/projects/gcolor2/files/gcolor2/0.4/gcolor2-0.4.tar.gz
- lxinput   [GPL2]  https://github.com/lxde/lxinput
- lxrandr   [GPL2]  https://github.com/lxde/lxrandr
- lxtask    [GPL2]  https://github.com/lxde/lxtask
- lxterminal [GPL2] https://github.com/lxde/lxterminal
- Xdialog    [GPL2] 
- yad        [GPL2] https://github.com/v1cont/yad/archive/refs/tags/v0.42.0.tar.gz


## Interesting apps to consider

- mdview [gpl2] http://distro.ibiblio.org/fatdog/source/800/mdview-2020-09-10.tar.bz2

## Requirements

- GLIB2 >= 2.32
- GTK2 >= 2.24 or GTK3 >= 3.14

## Compile & install
- ./configure --prefix=/usr
- make
- make install

## Compiling specific apps

If you only want to compile and install or test specific apps:

- ./configure --prefix=/usr --with-apps=asunder,isomaster

(only compile asunder and isomaster)

## Notes

- GTK2: `./configure --prefix=/usr --enable-gtk2`
- see also `./configure --help`
