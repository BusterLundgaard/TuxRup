# getting debug symbols and document names
for getting debug symbols, gdb will do it automatically for us if we just set add to .gdbinit in ~ the line 'set debuginfod enabled on'
we can make gdb commands from outside a gdb session by using --batch -ex
so we do gdb --batch -ex "info functions" /usr/bin/gnome-clocks to get a list of all functions in gnome-clocks and which documents theyre contained in

# getting source code (just on arch)
$ pacman -Qo /usr/bin/gnome-clocks
/usr/bin/gnome-clocks is owned by gnome-clocks 45.0-1

You now know the package — and from there, you can look up the source/download URL on:

    https://packages.archlinux.org

    Or use asp export gnome-clocks to get PKGBUILD

(but honestly this here is quite the hassle ... just download yourself for now)





