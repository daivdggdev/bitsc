#include <stdio.h>
#include <stdlib.h>
#include "bencode.h"
#include "../type.h"

int main(int argc, char *argv[])
{
	int i;

	setbuf(stdout, NULL);

	char *filepath = argv[1];
	printf("line: %d\n", __LINE__);

    // check
    if (!filepath) return -1; 
	printf("line: %d\n", __LINE__);

    // open
    FILE *fp = fopen(filepath, "rb");
    if (!fp)
	{
		perror("fopen failed!\n");
		return -1;
	}
	printf("line: %d\n", __LINE__);

    // calc file size
    fseek(fp, 0, SEEK_END);
    int32 filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
	printf("line: %d\n", __LINE__);

    // calloc
    char8 *meta = (char8*)calloc(1, filesize + 1);
    if (!meta) return -1; 

	printf("line: %d\n", __LINE__);
    // read file
    if (fread(meta, 1, filesize, fp) != filesize) return -1; 
	printf("line: %d\n", __LINE__);

	be_node *n = be_decode(meta);
	printf("DECODING: %s\n", argv[1]);
	if (n) {
		be_dump(n);
		be_free(n);
	} else
		printf("\tparsing failed!\n");

#if 0
	for (i = 1; i < argc; ++i) {
		be_node *n = be_decode(argv[i]);
		printf("DECODING: %s\n", argv[i]);
		if (n) {
			be_dump(n);
			be_free(n);
		} else
			printf("\tparsing failed!\n");
	}
#endif

	return 0;
}
