// Copyright 2012 Grazioli Giovanni <wargio@libero.it>
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

#define UNKN        0x0004 //UTF8
#define UTF8        0x0204
#define INTG        0x0404

#define HM          Playstation Network
#define GD          Game patch of disc game
#define CB          Network
#define HG          Harddrive
#define DG          Disc Game
#define AV          Video
#define AM          Music
#define AP          Photo
#define AT          TV
#define MN          Minis
#define AR          Patches
#define IP          Install Package
#define DP          Disc Package
#define TR          Theme DIR
#define VR          Video DIR
#define VI          Video
#define XR          Extra Contents

#define SFX_HDR		"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n<paramsfo add_hidden=\"false\">\n"
#define SFX_END		"</paramsfo>"

static int dbg = 0;
#define debug(...) if(dbg)printf(__VA_ARGS__)

typedef struct _sfo_entry {
    char* name;
    int   type;
    u64   vu64;
    char* utf8;
    u32   size;
} sfo_entry_t;

typedef struct _sfo_data {
    u32          length;
    sfo_entry_t* entries;
} sfo_data_t;

#define TYPE_VU64 1
#define TYPE_UTF8 2
#define set_size(o,n) (o).size=(n);(o).name=calloc((n),sizeof(char))
#define set_utf8(o,n) (o).type=TYPE_UTF8;(o).utf8=malloc((n))
#define set_vu64(o,n) (o).type=TYPE_VU64;(o).vu64=(n)

void build_sfx(const char* sfx_path, sfo_data_t* data) {
    printf("Writing SFX..\n");
    FILE *fp = fopen(sfx_path, "w");;

    if (!fp) {
        fail("Failed on writing SFX [Path: %s]", sfx_path);
    } else {
        fprintf(fp, SFX_HDR);
        u32 i;
        for (i = 0; i < data->length; i++) {
            sfo_entry_t* e = &data->entries[i];
            if (e->type == TYPE_VU64) {
                fprintf(fp, "  <param key=\"%s\" fmt=\"int32\" max_len=\"%u\">%lu</param>\n", e->name, e->size, e->vu64);
            } else {
                fprintf(fp, "  <param key=\"%s\" fmt=\"utf8\" max_len=\"%u\">%s</param>\n", e->name, e->size, e->utf8);
            }
        }
        fprintf(fp, SFX_END);
        fclose(fp);
    }
    printf("SFX Written!\n");
}

void read_sfo(u8* sfo, sfo_data_t* data) {
    u32 i, k;

    printf("Reading SFO..\n");
    u32 sfo_names    = le32(sfo + 0x08);
    u32 sfo_param    = le32(sfo + 0x0c);
    u32 sfo_n_params = le32(sfo + 0x10);

    u32 pname;
    u32 pnlen;
    u32 param;
    u32 psize;
    u16 type;

    data->length  = sfo_n_params;
    data->entries = calloc(data->length, sizeof(sfo_entry_t));

    debug("[SFO HDR]     0x%08x\n", (u32) be32(sfo));
    debug("[SFO Version] 0x%08x\n", (u32) le32(sfo + 0x4));
    debug("[SFO N]	      %u Value(s)\n", sfo_n_params);
    debug("[SFO Params Offsets]  0x%08x\n", sfo_param);
    debug("[ SFO ]\n");
    for (i = 0; i < sfo_n_params; i++) {
        if (i != sfo_n_params - 1) {
            pnlen = le8(sfo + (0x24 + (i * 0x10))) - le8(sfo + (0x14 + (i * 0x10)));
        } else {
            pnlen = 0x8;
        }

        pname = (sfo_names+ le16(sfo + (0x14 + (i * 0x10))));
        psize = le32(sfo + (0x1c + (i * 0x10)));
        param = le32(sfo + (0x20 + (i * 0x10))) + sfo_param;
        type  = le16(sfo + (0x16 + (i * 0x10)));


        set_size(data->entries[i], pnlen + 1);
        for (k = 0; k < pnlen; k++) {
            data->entries[i].name[k] = be8(sfo + (pname + k));
        }
        debug("[ %3i ] %16s", i + 1, data->entries[i].name);

        if (psize == 0x4) {
            if (type != UTF8) {
                set_vu64(data->entries[i], le32(sfo + param));
            } else {
                set_utf8(data->entries[i], psize + 1);
                for (k = 0; k < psize; k++) {
                    data->entries[i].utf8[k] = le8(sfo + param + k);
                }
            }
        } else if (psize > 0x4 && psize <= 0x8) {
            if (type != UTF8) {
                set_vu64(data->entries[i], be32(sfo + param));
            } else {
                set_utf8(data->entries[i], psize + 1);
                for (k = 0; k < psize; k++) {
                    data->entries[i].utf8[k] = le8(sfo + param + k);
                }
            }
        } else if (psize > 0x8 && psize <= 0x16) {
            if (type != UTF8) {
                set_vu64(data->entries[i], le32(sfo + param));
            } else {
                set_utf8(data->entries[i], psize + 1);
                for (k = 0; k < psize; k++) {
                    data->entries[i].utf8[k] = le8(sfo + param + k);
                }
            }
        } else if (psize > 0x16 && psize <= 0x30) {
            if (type != UTF8) {
                set_vu64(data->entries[i], le32(sfo + param));
            } else {
                set_utf8(data->entries[i], 0x31);
                for (k = 0; k < 0x30; k++) {
                    data->entries[i].utf8[k] = le8(sfo + param + k);
                }
            }
        } else if (psize >= 0x32) {
            if (type != UTF8) {
                set_vu64(data->entries[i], le32(sfo + param));
            } else {
                set_utf8(data->entries[i], psize + 1);
                for (k = 0; k < psize; k++) {
                    data->entries[i].utf8[k] = le8(sfo + param + k);
                }
            }
        }

        if (type != UTF8) {
            debug(" | Param: 0x%lx\n", data->entries[i].vu64);
        } else {
            debug(" | Param: %s\n", data->entries[i].utf8);
        }
    }
}

int main(int argc, char * argv[]) {
    u8* sfo = NULL;
    sfo_data_t data = {0};
    if (argc == 3) {
        sfo = (u8*) mmap_file(argv[1]);
        read_sfo(sfo, &data);
        build_sfx(argv[2], &data);
    } else if (argc == 4) {
        if (strcmp(argv[1], "-d") != 0) {
            fail("invalid option: %s", argv[1]);
        } else {
            dbg = 1;
        }

        sfo = (u8*) mmap_file(argv[2]);
        read_sfo(sfo, &data);
        build_sfx(argv[3], &data);
    } else {
        fail("usage: %s PARAM.SFO PARAM.SFX\n	-d\t| for more informations", argv[0]);
    }

    return 0;
}