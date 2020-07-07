ims alltype
start "ak1lib" ims ak1lib
start "ak2lib" ims ak2lib
start "ak3lib" ims ak3lib
start "ak4lib" ims ak4lib
start "kblib" ims kblib
start "utlib" ims utlib
start "kgglib" ims kgglib
start "bdlib" ims bdlib
start "utlib" imq utlib
start "splib" imq splib
if %RELVER:~1,2% GEQ 72 goto vers72
start "eolib" imq eolib
start "ralib" ims ralib
start ims vak91
start imf vta01
:vers72
start "sqlkern" imq sqlkern
start "sqltcp" imq sqltcp
start "sqlusr" imq sqlusr
start "parinlib" imq parinlib
start "cservlib" imq cservlib
start "eoxlib" imq eoxlib
start "eoxlib" imf eoxlib
start "splib" imf splib
if %RELVER:~1,2% EQU 73 goto vers73
start ims vak53
goto end
:vers73
start "RunTime" imq RunTime

:end

