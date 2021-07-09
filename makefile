all:
	/usr/local/dos-djgpp/i586-pc-msdosdjgpp/bin/g++ Main.cpp -lgrx20 -lpng -lz -lalleg -o bin/Release/main.exe
	make launch
launch:
	dosbox -c "mount p: $(CURDIR)/bin/Release" -c "p:" -c main