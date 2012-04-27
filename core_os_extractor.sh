#!/bin/bash
#Change this line "TOOLS=/home/wargio/.ps3tools" with the path of your tools, example: TOOLS=/home/god/ps3dev/ps3tools
TOOLS=/home/wargio/.ps3tools
if [ $# -eq 1 ]; then
	echo "Wargio's coreos extractor v0.1"
	echo "Just for 3.56+ PUPs"
	echo ""
	echo "Extracting PUP.."
	$TOOLS/pupunpack $1 PUP_TMP >> logs.txt
	rm -rf logs.txt
	cd PUP_TMP
	echo ""
	echo "Extracting TARs.."
	mkdir update_files
	cd update_files
	tar -xf ../update_files.tar
	echo ""
	echo "Extracting SCE PKGs.."
	$TOOLS/unpkg CORE_OS_PACKAGE* CORE_OStmp
	cd CORE_OStmp
	echo ""
	echo "Extracting CORE_OS.."
	$TOOLS/cosunpkg content CORE_OS >> log.txt
	echo ""
	echo "Almost finished.."
	cp -rf CORE_OS/ ../../../CORE_OS
	cd ../../../
	rm -rf PUP_TMP
	echo "Done..."
	echo "CORE_OS from" $1 "extracted..."
else
	echo "usage: "
	echo "	./extract_coreos <PUP>"
fi

