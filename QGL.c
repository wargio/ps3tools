#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <zlib.h>
#include "tools.h"
#include "types.h"

static u8 *qgl = NULL;

const u32 QRCF = 0x51524346;
const u32 QRCC = 0x51524343;

#define BUFFER_SIZE		0x80000

int Decompress(u8 *in, u32 in_len, u8 *out, u32 out_len){
	z_stream s;
	int ret;

	memset(&s, 0, sizeof(s));

	s.zalloc = Z_NULL;
	s.zfree = Z_NULL;
	s.opaque = Z_NULL;

	ret = inflateInit(&s);
	if (ret != Z_OK)
		fail("inflateInit returned %d", ret);

	s.avail_in = in_len;
	s.next_in = in;

	s.avail_out = out_len;
	s.next_out = out;

	ret = inflate(&s, Z_FINISH);
	if (ret != Z_OK && ret != Z_STREAM_END)
		fail("inflate returned %d", ret);

	inflateEnd(&s);
	return ret;
}

u8* Compress(u8 *in, u32 in_len, u32 *out_len){
	static u8 out[BUFFER_SIZE];
	z_stream s;
	int ret;
	memset(&s, 0, sizeof(s));

	s.zalloc = Z_NULL;
	s.zfree = Z_NULL;
	s.opaque = Z_NULL;

	ret = deflateInit(&s,9);
	if (ret != Z_OK)
		fail("deflateInit returned %d", ret);

	s.avail_in = in_len;
	s.next_in = in;

	s.avail_out = BUFFER_SIZE;
	s.next_out = out+0x8;

	ret = deflate(&s, Z_FINISH);
	if (ret != Z_OK && ret != Z_STREAM_END)
		fail("deflate returned %d", ret);
	*out_len = BUFFER_SIZE - s.avail_out +0x8;
	deflateEnd(&s);

	char hdr[] = {0x51, 0x52, 0x43, 0x43};
	memcpy(out,hdr,4);
	char size[4];
	size[0] = (uint8_t)(in_len >> 24);
	size[1] = (uint8_t)(in_len >> 16);
	size[2] = (uint8_t)(in_len >> 8);
	size[3] = (uint8_t)(in_len);
	memcpy(out+0x4,size,4);

	return out;
}


void decompress_qgl(const char* file, const char* fileout){
	if(be32(qgl)!=QRCC)
		fail("invalid file! not a qrc");
	u32 size_unpacked = be32(qgl+0x04);

	FILE *fp = fopen(file,"rb");
	fseek (fp, 0, SEEK_END);
	u32 size_file = ftell (fp);
	fclose(fp);

	printf("Decompressed file size: %d Bytes\n",size_unpacked);
	printf("File size:              %d Bytes\n",size_file);

	u8 qgl_dec[size_unpacked];
	Decompress(qgl+0x08, size_file-0x08, qgl_dec, size_unpacked);
	memcpy_to_file(fileout, qgl_dec, size_unpacked);
	printf("File Decompressed!! file %s created!\n",fileout);

}

void compress_qgl(const char* file, const char* fileout){
	if(be32(qgl)!=QRCF)
		fail("invalid file! not a qrc");
	u32 size_packed = 0;

	FILE *fp = fopen(file,"rb");
	fseek (fp, 0, SEEK_END);   // non-portable
	u32 size_file = ftell (fp);
	fclose(fp);

	printf("File size:            %d Bytes\n",size_file);

	u8 *qgl_comp = Compress(qgl, size_file, &size_packed);
	printf("Compressed file size: %d Bytes\n",size_packed);
	memcpy_to_file(fileout, qgl_comp, size_packed);
	printf("File Compressed!! file %s created!\n",fileout);
}

int main(int argc, char *argv[]){

	if (argc == 4) {
		qgl = mmap_file(argv[2]);
		if (strcmp(argv[1], "-x") == 0)
			decompress_qgl(argv[2],argv[3]);
		else if (strcmp(argv[1], "-c") == 0)
			compress_qgl(argv[2],argv[3]);
		else
			fail("invalid option: %s", argv[1]);


	} else {
		fail("usage: %s [OPTION: -x -c] file.qrc out_file.qrc\n"
		     "\t-x | decompress qrc\n"
		     "\t-c | compress qrc"
			,argv[0]);
	}


	return 0;
}
