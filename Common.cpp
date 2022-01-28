#ifndef Common_CPP
#define Common_CPP
#include <stdarg.h>
#include <unistd.h>
#include <fstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <time.h>
#include "Const.h"
#include "Common.h"


double clockToMilliseconds(clock_t ticks){
	// units/(units/time) => time (seconds) * 1000 = milliseconds
	return (ticks/(double)CLOCKS_PER_SEC)*1000.0;
}

char *currentDir(){
	char *buffer = (char*) malloc(sizeof(char)*1024);
	char *ptr = getcwd(buffer,1024);
	int i;
	for (i = 0; i < strlen(ptr); i++){
		if (buffer[i] == '/'){
			buffer[i] = '\\';
		}
	}
	return ptr;
}

void debugOut(std::string txt){
    std::ofstream file;
    file.open("DEBUG.LOG", std::ios_base::app);
    file << "\r\n" << txt;
    file.close();
}

template<typename T>
void debugOut(std::string label, T item){
	std::ofstream file;
    file.open("DEBUG.LOG", std::ios_base::app);
    file << "\r\n" << label << item;
    file.close();
}

#endif
