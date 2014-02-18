all:
	gcc -g main.c tracker/tracker.c metafile/metafile.c utils/polarssl/sha1.c -o main -I ./metafile -I ./utils -I./tracker
