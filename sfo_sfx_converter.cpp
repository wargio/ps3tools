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
#include <fstream>
using namespace std;

#define UNKN		0x0004 //UTF8
#define UTF8		0x0204
#define INTG		0x0404

#define HM		Playstation Network
#define GD		Game patch of disc game
#define CB		Network
#define HG		Harddrive
#define DG		Disc Game
#define AV		Video
#define AM		Music
#define AP		Photo
#define AT		TV
#define MN		Minis
#define AR		Patches
#define IP		Install Package
#define DP		Disc Package
#define TR		Theme DIR
#define VR		Video DIR
#define VI		Video
#define XR		Extra Contents

//#define SFX_HDR		<?xml version="1.0" encoding="utf-8" standalone="yes"?>
//#define SFX_INIT		<paramsfo add_hidden="false">
//#define SFX_END		</paramsfo>

static int dbg=0;

static u8 *sfo = NULL;
u32 sfo_header = 0x46535000; //Big endian
static u32 sfo_n_params;

static string	*SFO_VALUES;
static int	*content;	//1 int32	2 UTF8
static u64	*int32_params;
static string	*UTF8_params;
static u32	*length;
const char ii[] = {'"'};

void build_sfx(const char *sfx_path){
	printf("Writing SFX..\n");
	fstream f;
	f.open(sfx_path,ios::out);
	if(f.fail())
		fail("Failed on writing SFX [Path: %s]",sfx_path);

	else{
		f << "<?xml version=" << ii << 1.0 << ii << " encoding=" << ii << "utf-8" << ii << " standalone=" << "yes"<< ii <<"?>" << endl;
		f << "<paramsfo add_hidden=" << ii << "false" << ii << ">" << endl;
		     
		for(int i=0;i<(unsigned int)(u32)sfo_n_params;i++){
			if(content[i]!=2)
				f << "  <param key=" << ii << SFO_VALUES[i] << ii << " fmt=" << ii << "int32" << ii << " max_len=" << ii << length[i] << ii << ">" << int32_params[i] << "</param>"  << endl;
			else
				f << "  <param key=" << ii << SFO_VALUES[i] << ii << " fmt=" << ii << "utf8"  << ii << " max_len=" << ii << length[i] << ii << ">" << UTF8_params[i] << "</param>"  << endl;
		}
		f << "</paramsfo>";
		f.close();
	}
	printf("SFX Written!\n");
}

void read_sfo(){
	printf("Reading SFO..\n");
	u32 sfo_vals	 = le32(sfo+0x08);
	u32 sfo_param	 = le32(sfo+0x0c);
	u32 sfo_type;
	sfo_n_params     = le32(sfo+0x10);
	u32 sfo_val_ptr,sfo_val_size,sfo_param_ptr,sfo_param_size;
	u64 param32;
	unsigned int i,k;
	SFO_VALUES	= new string [sfo_n_params];
	content		= new int    [sfo_n_params];
	int32_params	= new u64    [sfo_n_params];
	UTF8_params	= new string [sfo_n_params];
	length		= new u32    [sfo_n_params];

	if(dbg) printf("[SFO HDR]     0x%08x\n", (unsigned int)be32(sfo));
	if(dbg) printf("[SFO Version] 0x%08x\n", (unsigned int)le32(sfo+0x4));
	if(dbg) printf("[SFO N]	      %u Value(s)\n", sfo_n_params);
	if(dbg) printf("[SFO Params Offsets]  0x%08x\n", sfo_param);
	if(dbg) printf("[ SFO ]\n");
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
		if(dbg) printf("[ %3i ] %16s",i+1, value);
		
		SFO_VALUES[i]	= value;
		length[i]	= sfo_param_size;

		if(sfo_param_size==0x4){
			if(sfo_type!=UTF8){
				param32 = le32(sfo+sfo_param_ptr);
			}else{
				for(k=0x0;k<sfo_param_size;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
			}
		}
		else if(sfo_param_size>0x4&&sfo_param_size<=0x8){
			if(sfo_type!=UTF8){
				param32 = be32(sfo+sfo_param_ptr);
			}else{
				for(k=0x0;k<sfo_param_size;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
			}
		}
		else if(sfo_param_size>0x8&&sfo_param_size<=0x16){
			if(sfo_type!=UTF8){
				param32 = be64(sfo+sfo_param_ptr);
			}else{
				for(k=0x0;k<sfo_param_size;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
			}
		}
		else if(sfo_param_size>0x16&&sfo_param_size<=0x30){
			if(sfo_type!=UTF8){
				param32 = be64(sfo+sfo_param_ptr);
			}else{
				for(k=0x0;k<0x30;k+=0x1)
					param[k] = le8(sfo+sfo_param_ptr+k);
			}
		}
		else if(sfo_param_size>=0x32){
			if(sfo_type!=UTF8){
				param32 = be64(sfo+sfo_param_ptr);
			}else{
				for(k=0x0;k<sfo_param_size;k++)
				param[k] = le8(sfo+sfo_param_ptr+k);
			}
		}else{}

		if(sfo_type!=UTF8){
			content[i]=1;
			int32_params[i]=param32;
			if(dbg) printf(" | Param: 0x%x\n",(unsigned int)param32);
		}else{
			content[i]=2;
			UTF8_params[i]=param;
			if(dbg) printf(" | Param: %s\n", param);
		}
	}
}

int main(int argc, char *argv[]){

	if (argc == 3) {
		sfo = (u8*)mmap_file(argv[1]);
		read_sfo();
		build_sfx(argv[2]);
	}else if(argc == 4) {
		if (strcmp(argv[1], "-d") != 0)
			fail("invalid option: %s", argv[1]);
		else
			dbg=1;

			sfo = (u8*)mmap_file(argv[2]);
			read_sfo();
			build_sfx(argv[3]);
	}else {
		fail("usage: %s PARAM.SFO PARAM.SFX\n	-d\t| for more informations",argv[0]);
	}


	return 0;
}
