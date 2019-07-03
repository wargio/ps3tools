#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dump_eid_data(FILE * pFile, int iInputSize, int iEidCount, char * pFilenamePrefix) {
    FILE* pOutput;
    char* szFilename;
    char* szBuf;
    int iRes, iSize;

    printf("dumping EID%d from eEID at %p, size %d (%x)..\n", iEidCount, pFile, iInputSize, iInputSize);

    szBuf = (char * ) malloc(iInputSize + 1);
    szFilename = (char * ) malloc(strlen(pFilenamePrefix) + 2);

    if (szBuf == NULL) {
        perror("malloc");
        exit(1);
    }

    iSize = fread(szBuf, iInputSize, 1, pFile);
    sprintf(szFilename, "%s%d", pFilenamePrefix, iEidCount);
    pOutput = fopen(szFilename, "wb");
    iRes = fwrite(szBuf, iInputSize, 1, pOutput);

    if (iRes != iSize) {
        perror("fwrite");
        exit(1);
    }

    free(szBuf);
}

void usage(const char * name) {
    printf("usage: %s <eEID> <EID name prefix>\n", name);
    exit(1);
}

int main(int argc, char ** argv) {
    FILE* pFile;
    char* pPrefix;

    if (argc != 3) {
        usage(argv[0]);
    }

    if (strlen (argv[1]) < 1 || strlen (argv[2]) < 1) {
        usage(argv[0]);
    }

    pPrefix = argv[2];
    pFile = fopen(argv[1], "rb");
    if (pFile == NULL) {
        usage(argv[0]);
    }

    fseek(pFile, 0x70, SEEK_SET);

    if (pPrefix != NULL) {
        dump_eid_data(pFile, 2144, 0, pPrefix);
        dump_eid_data(pFile, 672, 1, pPrefix);
        dump_eid_data(pFile, 1840, 2, pPrefix);
        dump_eid_data(pFile, 256, 3, pPrefix);
        dump_eid_data(pFile, 48, 4, pPrefix);
        dump_eid_data(pFile, 2560, 5, pPrefix);
    }
    return 0;
}