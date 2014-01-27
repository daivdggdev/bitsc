#include "parse_metafile.h"

int parse_metafile(const char8 *filepath)
{
	// check
	if (!filepath) return -1;

	// open
	FILE *fp = fopen(filepath, "rb");
	if (!fp) return -1;

	// calc file size
	fseek(fp, 0, SEEK_END);
	int32 filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// calloc
	char8 *meta = (char8*)calloc(1, filesize);
	if (!meta) return -1;

	// read file
	if (fread(meta, 1, filesize, fp) != filesize) return -1;

	return parse_metafile2(meta);
}

int parse_metafile2(const char8 *meta)
{
	if (!meta) return -1;


}
