#include "metafile.h"
#include "tracker.h"

int main(int argc, char **argv)
{
	meta_file_t meta_file;
	parse_metafile(argv[1], &meta_file);

	tracker_connect(&meta_file);
}
