// Copyright 2012       Grazioli Giovanni <wargio@libero.it>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "tools.h"
#include "little_endian.h"

static u8* HIS;
static u8* HIP;
static u32 size;
static FILE *fp; 


void conv(){
	u32 i;
	wbe32(HIS    ,0x01000000);
	wbe32(HIS+0x4,0x004ef659);
	wbe16(HIS+0x8,0x7002);

	fseek(fp, 0, SEEK_SET);
	fwrite(HIS,10, 1, fp);
	for(i=0;i<size;i++){
		fseek(fp,0x0a+i, SEEK_SET);
		fwrite(HIP+i, 1, 1, fp);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		fail("usage: %s PARAM.HIP PARAM.HIS ",argv[0]);
	HIP = mmap_file(argv[1]);
	struct stat data;
	stat(argv[1], &data);
	size = data.st_size;

	u8 tmp[size];
	HIS = tmp;
	fp = fopen(argv[2], "wb");

	if (fp == NULL)
		fail("fopen(%s)", argv[2]);

	conv();

	fclose(fp);

	return 0;
}
