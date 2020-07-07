#
# $DESC/install/perl/packages/syncman.com
#
# Maintainer: D036768
#

* ->$WRK/fast/install/packages/syncman/ ascii concom=yes
PACKAGEDATA setbuildinfo dep=:config/Buildinfo/VERSIONS uncond

syncman.lst ascii ->WRK/fast/install/packages/syncman/ <-$INSTROOT/etc/syncman.lst

