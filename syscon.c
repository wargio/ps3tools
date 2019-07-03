#include "syscon_m.h"
#include "tools.h"
#include "types.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

u8 *pkg = NULL;
static u64 dec_size;
static u32 meta_offset;
static u32 n_sections;
static u32 sc_fw_size;
const  u32 sc_header = 0x1b2d700f ;


struct sc_hdr{
	u32 magic;              /* 0x1b2d700f      */
	u32 field0[8];          /* checksum??      */
	u32 offset0;            /* header size?    */
	u32 size0;              /* firmware size   */
	u32 size1;              /* binary size     */
	u32 field1[4];          /* checksum binary */
};


void print_fw_struct_info(struct sc_hdr *H){
	printf("[CHECKSUM 1] %08x %08x %08x %08x\n"
	       "             %08x %08x %08x %08x\n",
		H->field0[0], H->field0[1], H->field0[2], H->field0[3],
		H->field0[4], H->field0[5], H->field0[6], H->field0[7]);
	printf("[NULL    ??] %08x\n",H->offset0);
	printf("[Firm  SIZE] %u Bytes\n",H->size0);
	printf("[Bin SIZE]   %u Bytes\n",H->size1);
	printf("[CHECKSUM 2] %08x %08x %08x %08x\n",
		H->field1[0], H->field1[1], H->field1[2], H->field1[3]);
}


static void unpack_content(const char *name)
{
	u8 *tmp;
	u8 *decompressed;
	u64 offset;
	u64 size;
	u64 size_real;

	tmp = pkg + meta_offset + 0x80 + 0x30 * 2;


	offset = be64(tmp);
	size = be64(tmp + 8);
	size_real = dec_size - 0x80;

	printf("[FW size] %u Bytes\n", (u32)size);
	sc_fw_size = size;
	if (be32(tmp + 0x2c) == 0x2) {
		decompressed = malloc(size_real);
		memset(decompressed, 0xaa, size_real);

		decompress(pkg + offset, size, decompressed, size_real);

		memcpy_to_file(name, decompressed, size_real);
	} else {
		memcpy_to_file(name, pkg + offset, size);
	}
}


static void decrypt_pkg(void)
{
	int j;

	//u16 flags;
	u16 type;
	//u32 hdr_len;

	u32 PatchID1;
	u32 PatchID2;
	u16 SoftID;

	struct keylist *k;

	//flags	= be16(pkg + 0x08);
	type	 = be16(pkg + 0x0a);
	//hdr_len  = be64(pkg + 0x10);
	dec_size = be64(pkg + 0x18);
	SoftID	 = be16(pkg + 0x28e);
	PatchID1 = be32(pkg + 0x290);
	PatchID2 = be32(pkg + 0x294);

	for(j=0;j<11;j++){
		if(SoftID==Get_Soft_ID(j)){
			printf("[Gen  %02i]", j+1);
			if(j<8) printf("[Phat]\n");
			else	printf("[Slim]\n");

			break;
		}if(j==10){
			printf("[New Gen][Slim]\n");
		}
	}


	printf("[Soft ID] %04X\n", SoftID);
	printf("[PatchID] %08X%08X\n", PatchID1, PatchID2);

	if (type != 3)
		fail("[!] not a .pkg file");

	k = keys_get(KEY_PKG);

	if (k == NULL)
		fail("[!] no key found");

	if (sce_decrypt_header(pkg, k) < 0)
		fail("[!] pkg header decryption failed");

	if (sce_decrypt_data(pkg) < 0)
		fail("[!] pkg data decryption failed");

	meta_offset = be32(pkg + 0x0c);
	n_sections  = be32(pkg + meta_offset + 0x60 + 0xc);

	if (n_sections != 3)
		fail("[!] invalid section count: %d", n_sections);

}
/*
u16 Get_Soft_ID(int i){
	u16 sc_soft_id[12];
	sc_soft_id[ 0] = 0x0B8E;
	sc_soft_id[ 1] = 0x0C16;
	sc_soft_id[ 2] = 0x0D52;
	sc_soft_id[ 3] = 0x0DBF;
	sc_soft_id[ 4] = 0x0E69;
	sc_soft_id[ 5] = 0x0F29;
	sc_soft_id[ 6] = 0x0F38;
	sc_soft_id[ 7] = 0x065D;
	sc_soft_id[ 8] = 0x0832;
	sc_soft_id[ 9] = 0x08C2;
	sc_soft_id[10] = 0x0918;

//decr
	sc_soft_id[11] = 0x08A0;

	return sc_soft_id[i];
}
*/
void readFW(void){
	u32 hdr;
	struct sc_hdr SC;
	memset(&SC, 0, sizeof(SC));
	hdr	= be32(pkg);

	SC.field0[0]	= be32(pkg + 0x4);
	SC.field0[1]	= be32(pkg + 0x8);
	SC.field0[2]	= be32(pkg + 0xc);
	SC.field0[3]	= be32(pkg + 0x10);
	SC.field0[4]	= be32(pkg + 0x14);
	SC.field0[5]	= be32(pkg + 0x18);
	SC.field0[6]	= be32(pkg + 0x1c);
	SC.field0[7]	= be32(pkg + 0x20);

	SC.offset0	= le32(pkg + 0x24); // Where the header starts???
	SC.size0	= le32(pkg + 0x28); // FW size
	SC.size1	= le32(pkg + 0x2c); // FW size - header size

	SC.field1[0]	= be32(pkg + 0x30);
	SC.field1[1]	= be32(pkg + 0x34);
	SC.field1[2]	= be32(pkg + 0x38);
	SC.field1[3]	= be32(pkg + 0x3c);

	if(hdr!=sc_header)
		printf("[New HDR] %08X \n", hdr);

	print_fw_struct_info(&SC);

}

int header_check(){
	if(sc_header != be32(pkg))
		return -1;
	else
		return 1;
}

int main(int argc, char *argv[])
{
	if (argc == 2) {
		pkg = mmap_file(argv[1]);
		if(header_check()<0){
			printf("\n[Name FW] %s\n", argv[1]);
			decrypt_pkg();
			unpack_content("temp.sc");
			pkg = mmap_file("temp.sc");
			readFW();

			if(remove("temp.sc") != 0)
				fail("[!] Error deleting temp file.");
		}else{
			printf("\n[Name FW] %s\n", argv[1]);
			printf("[!] Unknown firmware [!]\n");
			readFW();
		}
	} else {
		fail("usage: unpkg syscon_firmware.pkg");
	}


	return 0;
}
