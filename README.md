# dos-gui-menu
A graphical menu for games in MS-DOS


Item Configuration
=============

* Menu items are defined by a "_menu.cfg" file in the game directory
* The menu item name, image, description and other attributes are defined in this file. Files (readme, image) referenced in this config are relative to the folder.
* The program will scan for menu configuration files at startup first time.

NOTE: There's no rescan option built into the program yet. To force a full re-scan delete MNUCACHE.DB from the program's directly.

I went with this model so that folders could be re-arranged, removed, moved to another drive, etc - without having to modify a central database.

*Example File*
```
[Monkey Island 2: LeChuck's Revenge]
path=MONKEY2.EXE
image=monkey2.png
genre=Adventure
notes=test
favorite=T
readme=
developer=Lucasfilm Games
year=1992
cd=F
description=Monkey Island 2: LeChuck's Revenge is an adventure game developed and published by LucasArts in 1991. A sequel to 1990's The Secret of Monkey Island, it is the second game in the Monkey Island series. It was the sixth LucasArts game to use the SCUMM engine, and the first game to use the iMUSE sound system. In it, pirate Guybrush Threepwood searches for the legendary treasure of Big Whoop and again faces off against the pirate LeChuck, who is now a zombie.
```



Images
------

The image referenced in the menu config file should be a PNG, with a reduced number of colors (probably anything less than 200 colors). The program will redefine the palette to accomodate whatever colors are in the image, but the program operates in 256 color mode, and some colors are reserved by the program itself.

If your image appears with lots of black areas, it probably has too many colors and the palette has been exhausted.

Photoshop or GIMP are good tools for preparing the images. Both allow you to select a number of colors and create an optimum palette for the image.

Program Configuration
=====================

Configure program options by modifying the menu.cfg file located in the menu program directory.

*Example Config*
```
[scan]
#List of folders to scan for game definition files
C:\GAMES\

[options]
#resolution - high or low : high is 800x600, low is 320x200.
resolution=high
#diagnosticOverlay - true for FPS,lag and memory usage
diagnosticOverlay=false
```

Scan Folders
------------
Put each folder on a separate line.

To force a re-scan, delete the MNUCACHE.DB file from the program directory. The re-scan will occur on startup.


Low or High Resolution
----------------------

The menu program has two resolution modes.
* High - 800x600 at 256 colors. This requires a faster PC and video card capable of this mode.
* Low - 320x200 at 256 colors. This is standard VGA and should run on slower machines

Modify this setting in menu.cfg in the folder containing menu.bat/main.exe


Launching the Menu
==================
To avoid taking any memory away from games this program will unload itself before launching a game. It's important to run the menu using the batch file MENU.BAT




