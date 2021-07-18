all:
	/usr/local/dos-djgpp/i586-pc-msdosdjgpp/bin/g++ Main.cpp -lgrx20 -lpng -lz -lalleg -o bin/Release/main.exe
	/usr/local/dos-djgpp/i586-pc-msdosdjgpp/bin/g++ CharEditor/CharEdit.cpp -lgrx20 -lpng -lz -o bin/Release/CharEdit.exe
	make launch
launch:
	dosbox -c "mount p: $(CURDIR)/bin/Release" -c "p:" -c main

charedit:
	/usr/local/dos-djgpp/i586-pc-msdosdjgpp/bin/g++ CharEditor/CharEdit.cpp -lgrx20 -lpng -lz -o bin/Release/CharEdit.exe
	make launchchar
launchchar:
	dosbox -c "mount p: $(CURDIR)/bin/Release" -c "p:" -c charedit