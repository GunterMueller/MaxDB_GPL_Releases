&if $OSSPEC in [ MI_ULT ]
#!/bin/sh5
&else
&if $OSSPEC in [ DYNIX ]
#!/bin/sh
&else
:
&endif
&endif
# @(#)mf00ccc  3.1.3  1994-01-28
#
# create a v..00ccc module from a v..00 pascal include module
#
# TOOL          directory for tool scripts
# KEEPFLG       optional keep intermediate files flag
#

USAGE="usage: $0 [-<symbol>=<value>] [-k] unpacked-include-modules "

set -e

. toolvars
. mf_opts

if [ $# -lt 1 ]
then
    echo "$USAGE"
    exit 1
fi

CONS=
TYPS=
for ARG
do
    case "$ARG" in

    *.con )
        CONS="$CONS $ARG"
        ;;
    *.typ )
        TYPS="$TYPS $ARG"
        ;;
    * )
        echo "illegal input file: $ARG"
        exit 2
        ;;
    esac
done
# ARG contains the last argument
ARG=`basename "$ARG"`
ARG=`expr "$ARG" \: '\(.*\)\.' \| "$ARG"`

#
#  Create the new file with the suffix of 'ccc'
#
ARG=${ARG}ccc

PASTYP=
PASOPT=
#PASTYP="$TOOL/inp/pastyp"
#PASOPT="$TOOL/inp/pasopt.f"


$TOOL/pgm/mfcat $ARG.cns 'CONST' $CONS
$TOOL/pgm/mfcat $ARG.tps 'TYPE' $PASTYP $TYPS

cat $PASOPT $ARG.cns $ARG.tps > $ARG.puc

$TOOL/pgm/mfconcom -n $CONCOMOPTS $ARG.puc $ARG.p \
    "$MACH" "$KIND" "$TRACE" "$COMP" "$OS" "$OSSPEC"

comppc -Kt $ARG.p	# creates $ARG.c
$TOOL/pgm/ityp00 $ARG	# creates $ARG.con and $ARG.typ

cat $ARG.con $ARG.c $ARG.typ |egrep -v '^#include|^#ident' >$ARG

if [ -z "$KEEPFLG" ]
then
    rm -f $ARG.cns $ARG.tps $ARG.puc $ARG.p $ARG.con $ARG.typ $ARG.c 
fi

