// Copyright 2012       Grazioli Giovanni <wargio@libero.it>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tools.h"
#include "types.h"
#include "little_endian.h"

#define UNKN		0x0004 //UTF8
#define UTF8		0x0204
#define INTG		0x0404

#define HM	Playstation Network
#define GD	Game patch of disc game
#define CB	Network
#define HG	Harddrive
#define DG	Disc Game
#define AV	Video
#define AM	Music
#define AP	Photo
#define AT	TV
#define MN	Minis
#define AR	Patches
#define IP	Install Package
#define DP	Disc Package
#define TR	Theme DIR
#define VR	Video DIR
#define VI	Video
#define XR	Extra Contents

static u8 *sfo = NULL;
u32 sfo_header = 0x46535000; //Big endian

void read_sfo(){
	u32 sfo_vals	 = le32(sfo+0x08);
	u32 sfo_param	 = le32(sfo+0x0c);
	u32 sfo_type;
	u32 sfo_n_params = le32(sfo+0x10);
	u32 sfo_val_ptr,sfo_val_size,sfo_param_ptr,sfo_param_size;
	u64 param32;
	unsigned int i,k;


	printf("[SFO HDR]     0x%08x\n", (unsigned int)le32(sfo));
	printf("[SFO Version] 0x%08x\n", (unsigned int)le32(sfo+0x4));
	printf("[SFO N]	      %u Value(s)\n", sfo_n_params);
	printf("[SFO Values]  0x%08x\n", sfo_vals);
	printf("[SFO Params]  0x%08x\n", sfo_param);
	printf("[ SFO ]\n");
	for(i=0x0;i<sfo_n_params;i+=0x1){
		char value[0x20];
		char param[0x200];
		if(i!=sfo_n_params-1)
			sfo_val_size		= le8 (sfo+(0x24+(i*0x10)))-le8(sfo+(0x14+(i*0x10)));
		else
			sfo_val_size		= 0x8;
		sfo_val_ptr		= (sfo_vals+ le16(sfo+(0x14+(i*0x10))));
		sfo_param_size		= le32(sfo+(0x1c+(i*0x10)));
		sfo_param_ptr		= le32(sfo+(0x20+(i*0x10)))+sfo_param;
		sfo_type 		= le16(sfo+(0x16+(i*0x10)));

		for(k=0;k<(unsigned int)(u32)sfo_val_size;k++)
			value[k] = be8(sfo+(sfo_val_ptr+k));
		printf("[ %3i ] %16s",i+1, value);
		if(sfo_param_size==0x4){
			if(sfo_type!=UTF8){
				param32 = le32(sfo+sfo_param_ptr);
				printf(" | Param: 0x%x\n",(unsigned int)param32);
			}else{
				for(k=0x0;k<sfo_param_size;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
				printf(" | Param: %s\n", param);
			}
		}
		else if(sfo_param_size>0x4&&sfo_param_size<=0x8){
			if(sfo_type!=UTF8){
				param32 = be32(sfo+sfo_param_ptr);
				printf(" | Param: 0x%x\n",(unsigned int)param32);
			}else{
				for(k=0x0;k<sfo_param_size;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
				printf(" | Param: %s\n", param);
			}
		}
		else if(sfo_param_size>0x8&&sfo_param_size<=0x16){
			if(sfo_type!=UTF8){
				param32 = be64(sfo+sfo_param_ptr);
				printf(" | Param: 0x%x\n",(unsigned int)param32);
			}else{
				for(k=0x0;k<sfo_param_size;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
				printf(" | Param: %s\n", param);
			}
		}
		else if(sfo_param_size>0x16&&sfo_param_size<=0x30){
			if(sfo_type!=UTF8){
				param32 = be64(sfo+sfo_param_ptr);
				printf(" | Param: 0x%x\n",(unsigned int)param32);
			}else{
				for(k=0x0;k<0x30;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
				printf(" | Param: %s\n", param);
			}
		}
		else if(sfo_param_size>=0x32){
			if(sfo_type!=UTF8){
				param32 = be64(sfo+sfo_param_ptr);
				printf(" | Param: 0x%x\n",(unsigned int)param32);
			}else{
				for(k=0x0;k<sfo_param_size;k++)
				param[k] = le8(sfo+sfo_param_ptr+k);
				printf(" | Param: %s\n", param);
			}
		}else{}
	}
}

int main(int argc, char *argv[]){

	if (argc == 2) {
		sfo = mmap_file(argv[1]);
		read_sfo();

	} else {
		fail("usage: %s PARAM.SFO",argv[0]);
	}


	return 0;
}
