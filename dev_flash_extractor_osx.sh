TOOLS=/Users/wargio/.ps3tools
echo "Wargio's dev_flash & dev_flash3 extractor v0.3 (For OSX)"
echo "Just for 3.56+ PUPs"
echo ""

if [ $# -eq 1 ]; then
	echo "Extracting PUP.."
	$TOOLS/pupunpack $1 TMP >> logs.txt
	rm -rf logs.txt
	cd TMP
	mkdir update_files
	cd update_files
	tar -xf ../update_files.tar
	echo "Working now on dev_flash.."
	echo "Renaming files.."
	for a in {0..9}; do
	    if [ -f dev_flash_00$a* ]; then
	    	mv dev_flash_00$a* dev_flash_00$a.tar
	    fi
	done
	for a in {10..30}; do
	    if [ -f dev_flash_0$a* ]; then
	    	mv dev_flash_0$a* dev_flash_0$a.tar
	    fi
	done
	echo "Extracting SCE TARs.."
	for a in {0..9}; do
	    if [ -f dev_flash_00$a.tar ]; then
	    	$TOOLS/unpkg dev_flash_00$a.tar dev_flash_00$a
	    fi
	done
	for a in {10..30}; do
	    if [ -f dev_flash_0$a.tar ]; then
	    	$TOOLS/unpkg dev_flash_0$a.tar dev_flash_0$a
	    fi
	done
	for a in {0..9}; do
	    if [ -d dev_flash_00$a ]; then
	    	mv dev_flash_00$a/content dev_flash_00$a/content.tar
	    fi
	done
	for a in {10..30}; do
	    if [ -d dev_flash_0$a ]; then
	    	mv dev_flash_0$a/content dev_flash_0$a/content.tar
	    fi
	done
	echo "Extracting TARs.."
	for a in {0..9}; do
	    if [ -d dev_flash_00$a ]; then
	    	cd dev_flash_00$a
	    	tar -xf ./content.tar
	    	cd ..
	    fi
	done
	for a in {10..30}; do
	    if [ -d dev_flash_0$a ]; then
	   	 cd dev_flash_0$a
	    	tar -xf ./content.tar
	    	cd ..
	    fi
	done
	echo "Merging files.."
	mkdir dev_flash
	for a in {0..9}; do
	    if [ -d dev_flash_00$a ]; then
	    	cp -rf -n dev_flash_00$a/dev_flash ./
	    fi
	done
	for a in {10..30}; do
	    if [ -d dev_flash_0$a ]; then
	    	cp -rf -n dev_flash_0$a/dev_flash ./
	    fi
	done

	echo "Deleting dev_flash working dirs.."
	for a in {0..9}; do
	    rm -rf dev_flash_00$a
	done
	for a in {10..30}; do
	    rm -rf dev_flash_0$a
	done
	echo "Done.."
	echo "dev_flash extracted.."
	echo ""
	echo "Working now on dev_flash3.."
	echo "Renaming file.."
	mv dev_flash3* dev_flash3.tar
	echo "Extracting SCE TARs.."
	$TOOLS/unpkg dev_flash3.tar dev_flash3_ex
	echo "Renaming file again.."
	mv dev_flash3_ex/content dev_flash3_ex/content.tar
	echo "Extracting TARs.."
	tar -xf ./dev_flash3_ex/content.tar
	echo "Deleting dev_flash3 working dirs.."
	rm -rf dev_flash3_tmp
	echo "Copying dev_flash and dev_flash3"
	cp -rf dev_flash ../../dev_flash
	cp -rf dev_flash3 ../../dev_flash3
	cd ../..
	rm -rf TMP
	echo "Done..."
else

	echo "usage: "
	echo "	./unpack_dev_flash.sh <PUP>"
fi

