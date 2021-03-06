start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_11_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_12_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_13_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_14_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_15_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_16_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_17_data.txt -E 10
start /b loadercli -d targetdb -u benchmark,initial -cs ; -b import_18_data.txt -E 10

REM 320000 rows = 0,08 GB
REM 1:       1      40000
REM 2:    40001     80000
REM 3:    80001    120000
REM 4:   120001    160000
REM 5:   160001    200000
REM 6:   200001    240000
REM 7:   240001    280000
REM 8:   280001    320000

REM 3200000 rows = 0,8 GB
REM 1:        1    400000
REM 2:   400001    800000
REM 3:   800001   1200000
REM 4:  1200001   1600000
REM 5:  1600001   2000000
REM 6:  2000001   2400000
REM 7:  2400001   2800000
REM 8:  2800001   3200000

REM 32000000 rows = 8 GB
REM 1:         1  4000000
REM 2:   4000001  8000000
REM 3:   8000001 12000000
REM 4:  12000001 16000000
REM 5:  16000001 20000000
REM 6:  20000001 24000000
REM 7:  24000001 28000000
REM 8:  28000001 32000000
