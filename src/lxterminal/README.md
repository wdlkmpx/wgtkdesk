# LXTerminal fork

https://github.com/lxde/lxterminal
Starting point:
    commit 0febe16c2f97d75df04936df4ef34667655f084a
    Date:   Fri Dec 3 16:49:59 2021 -0500
        Allow tab width to be set via "Edit -> Preferences -> Advanced".

LXTerminal is a VTE-based terminal emulator with support for multiple tabs.  It
is completely desktop-independent and does not have any unnecessary
dependencies. In order to reduce memory usage and increase the performance all
instances of the terminal are sharing a single process.

### Dependencies

* libglib2.0-dev
* libgtk2.0-dev
* libvte-dev

## Authors (from the AUTHORS file)

- Fred Chien <cfsghost@gmail.com>
- Alessandro Pellizzari <alex_shu@users.sourceforge.net>
- Micahel <metsger@users.sourceforge.net>
- Yotam Medini <yotam.medini@gmail.com>
- Liu Yubao <yubao.liu@gmail.com>
- Marty Jack <martyj19@comcast.net>
- Martin Bagge <brother@bsnet.se>
- Icon: Taken from "nuoveXT 2" icon theme created by Alexandre Moore (saki)
