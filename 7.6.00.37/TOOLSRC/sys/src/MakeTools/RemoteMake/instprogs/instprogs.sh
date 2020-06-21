#!/bin/sh

TOOL=<TOOL>

rm $TOOL/bin/instlserver $TOOL/bin/instdbmsrv $TOOL/bin/instdbmverifypw
cd $TOOL/src/to_install
cp instlserver instdbmsrv instdbmverifypw $TOOL/bin
chown root $TOOL/bin/instlserver $TOOL/bin/instdbmsrv $TOOL/bin/instdbmverifypw
chmod 755 $TOOL/bin/instlserver $TOOL/bin/instdbmsrv $TOOL/bin/instdbmverifypw
chmod u+s $TOOL/bin/instlserver $TOOL/bin/instdbmsrv $TOOL/bin/instdbmverifypw

