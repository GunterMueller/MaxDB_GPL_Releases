imf alltype
start imq ak1lib
start imq ak2lib
start imq ak3lib
start imq ak4lib
start imq kblib
start imq utlib
start imq kgglib
start imq bdlib
start imq splib
if %RELVER:~1,2% GEQ 72 goto vers72
start imq eolib
start imq ralib
:vers72
start imq sqlkern
start imq sqltcp
start imq parinlib
start imq cservlib
start imq eoxlib
start imq splib
