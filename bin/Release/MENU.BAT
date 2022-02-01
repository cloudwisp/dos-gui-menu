@echo off
:START
if exist menuitem.bat GOTO DELMENUITEM
GOTO LOADMENU

:DELMENUITEM
        del menuitem.bat

:LOADMENU
MAIN %1
if exist menuitem.bat GOTO LOADGAME
GOTO END

:LOADGAME
        CALL menuitem.bat
        GOTO START

:END
