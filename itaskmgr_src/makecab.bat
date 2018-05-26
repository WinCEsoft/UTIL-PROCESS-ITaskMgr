@echo on

set BUILDER="C:\Program Files\Microsoft eMbedded Tools\Common\EVC\Bin\EVC.EXE"
set CABBUILDER="\Windows CE Tools\wce211\MS HPC Pro\support\appinst\bin\cabwiz.exe"
set TARGETDIR=.\
set TARGET=itaskmgr

set srcdir=itaskmgr_src
set cabdir=itaskmgr_cab

%BUILDER% %TARGETDIR%%TARGET%.vcw /make "%TARGET% - Win32 (WCE ARM) Release" /rebuild
%BUILDER% %TARGETDIR%%TARGET%.vcw /make "%TARGET% - Win32 (WCE MIPS) Release" /rebuild
%BUILDER% %TARGETDIR%%TARGET%.vcw /make "%TARGET% - Win32 (WCE SH3) Release" /rebuild
%BUILDER% %TARGETDIR%%TARGET%.vcw /make "%TARGET% - Win32 (WCE SH4) Release" /rebuild

mkdir arm
mkdir mips
mkdir sh3
mkdir sh4
mkdir common

copy %TARGETDIR%%TARGET%_arm.exe .\arm\%TARGET%.exe
copy %TARGETDIR%%TARGET%_mips.exe .\mips\%TARGET%.exe
copy %TARGETDIR%%TARGET%_sh3.exe .\sh3\%TARGET%.exe
copy %TARGETDIR%%TARGET%_sh4.exe .\sh4\%TARGET%.exe

REM ------ CABS ------ 

%cabbuilder% %TARGET%.inf /err %TARGET%.log /cpu ARM MIPS SH3 SH4 PXA
type %TARGET%.log

del .\%TARGET%.arm.dat
del .\%TARGET%.mips.dat
del .\%TARGET%.sh3.dat
del .\%TARGET%.sh4.dat

mkdir %cabdir%
move .\%TARGET%*.cab .\%cabdir%


REM ------ SRC ------ 
mkdir %srcdir%
copy *.c* .\%srcdir%
copy *.h* .\%srcdir%
copy *.vcw .\%srcdir%
copy *.vcp .\%srcdir%
copy *.vco .\%srcdir%
copy *.txt .\%srcdir%
copy *.rc .\%srcdir%
copy *.bat .\%srcdir%
copy *.inf .\%srcdir%
copy *.ico .\%srcdir%
copy *.html .\%srcdir%

lha32 u -rx %TARGET%_src.lzh .\%srcdir%\*.*
lha32 u -rx %TARGET%_cab.lzh .\%cabdir%\*.*
