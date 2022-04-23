all:
	/usr/local/dos-djgpp/i586-pc-msdosdjgpp/bin/g++ -w Main.cpp -lgrx20 -lpng -lz -o bin/Release/main.exe
	bin/Release/dump.sh
	#make launch
launch:
	dosbox -c "mount p: $(CURDIR)/bin/Release" -c "p:" -c menu.bat