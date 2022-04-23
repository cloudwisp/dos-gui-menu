#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
/usr/local/dos-djgpp/bin/i586-pc-msdosdjgpp-objdump -d -M intel -S "$SCRIPT_DIR/main.exe" > "$SCRIPT_DIR/main.dmp"