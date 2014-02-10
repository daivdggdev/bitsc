all:
	gcc -g main.c tracker/tracker.c metafile/parse_metafile.c utils/sha1/sha1.c -o main -I ./metafile -I ./utils -I./utils/sha1 -I./tracker
