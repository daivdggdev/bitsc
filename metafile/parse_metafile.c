#include "parse_metafile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sha1.h"

#define META_FILE_ANNOUNCE_TYPE                "8:announce"
#define META_FILE_ANNOUNCE_LIST_TYPE           "13:announce-list"

#define META_FILE_FILES_TYPE                   "5:files"

int32 get_info_hash(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	int32 push_pop = 0;
	char8 *p = NULL;
	char8 *b = NULL;
	char8 *e = NULL;

	int32 filesize = meta_file->meta_size;

	if (p = strstr(meta, "4:info"))
	{
		p += strlen("4:info");
		b = p;

		while (*p != '\0')
		{
			if(*p == 'd') 
			{ 
				push_pop++;
				p++;
			} 
			else if(*p == 'l') 
			{
				push_pop++;
				p++;
			} 
			else if(*p == 'i') 
			{
				p++;  // skip i
				if((p - meta) == filesize)  return -1;
				while(*p != 'e') {
					if((p - meta +1) == filesize)  return -1;
					else p++;
				}
				p++;  // skip e
			} 
			else if((*p >= '0') && (*p <= '9')) 
			{
				int32 number = 0;
				while((*p >= '0') && (*p <= '9')) 
				{
					number = number * 10 + (*p - '0');
					p++;
				}
				p++;  // skip :
				p = p + number;
			} 
			else if(*p == 'e') 
			{
				push_pop--;
				if(push_pop == 0) { e = p; break; }
				else  p++; 
			} 
			else 
			{
				return -1;
			}
		}

		if((p - meta) == filesize)  return -1;

		// sha1
		SHA1Context sha;
		SHA1Reset(&sha);
		SHA1Input(&sha, (const uchar8 *)b, e - b + 1);
		if (!SHA1Result(&sha))
		{
			printf("ERROR-- could not compute message digest");
			return -1;
		}
		memcpy(meta_file->info_hash, sha.Message_Digest, 20);
	}

	return p ? 0 : -1;
}

int32 get_peer_id(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	srand(time(NULL));
	sprintf(meta_file->peer_id,"-TT1000-%12d",random());

	return 0;
}

int32 get_file_length(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	char8 *p   = meta;
	uint64 len = 0;

	if (p = strstr(p, "6:length"))
	{
		p += strlen("6:length");

		// skip 'i'
		p++;

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');
			p++;
		}

		if (meta_file->file) meta_file->file->file_len = len;
	}

	return p ? 0 : -1;
}

int32 get_file_length_path(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	char8 *p               = meta;
	uint64 len             = 0;
	meta_file_file_t *file = NULL;
	meta_file_file_t *fp   = NULL;

	while (p = strstr(p, "6:length"))
	{
		p += strlen("6:length");
		len = 0;

		// skip 'i'
		p++;

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');
			p++;
		}

		file = (meta_file_file_t*)calloc(1, sizeof(meta_file_file_t));
		file->file_len = len;
		file->next     = NULL;

		if (meta_file->file == NULL) 
		{
			meta_file->file = file;
		}
		else
		{
			fp = meta_file->file;
			while (fp->next != NULL) fp = fp->next;
			fp->next = file;
		}
	}

	p   = meta;
	fp  = meta_file->file;
	while ((p = strstr(p, "4:path")) && fp)
	{
		p += strlen("4:path");
		len = 0;

		// skip 'l'
		p++;

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');
			p++;
		}

		// skip ':'	
		p++;

		memcpy(fp->file_name, p, len);	
		fp = fp->next;
	}
	
	return 0;
}

int32 get_file_mode(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	if (strstr(meta, META_FILE_FILES_TYPE)) meta_file->is_multi_file = 1;	

	return 0;
}

int32 get_file(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	char8 *p  = NULL;
	int32 len = 0;

	get_file_mode(meta, meta_file);

	if (p = strstr(meta, "4:name"))
	{
		p += strlen("4:name");

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');
			p++;
		}

		// skip ':'
		p++;

		if (meta_file->is_multi_file)
		{
			memcpy(meta_file->floder_name, p, len);	
			return get_file_length_path(meta, meta_file);
		}
		else
		{
			if (!meta_file->file) 
				meta_file->file = (meta_file_file_t*)calloc(1, sizeof(meta_file_file_t));

			memcpy(meta_file->file->file_name, p, len);	
			return get_file_length(meta, meta_file);
		}
	}

	return -1;
}

int32 get_pieces(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	char8 *p  = NULL;
	int32 len = 0;

	if (p = strstr(meta, "6:pieces"))
	{
		p += strlen("6:pieces");

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');	
			p++;
		}

		// skip ':'
		p++;

		meta_file->pieces = (char8*)calloc(1, len + 1);
		memcpy(meta_file->pieces, p, len);
	}

	return p ? 0 : -1;
}

int32 get_piece_length(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	char8 *p  = NULL;
	int32 len = 0;

	if (p = strstr(meta, "12:piece length"))
	{
		p += strlen("12:piece length");

		// skip 'i'
		p++;

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');	
			p++;
		}

		meta_file->piece_length = len;
	}

	return p ? 0 : -1;
}

int32 get_announce(const char8 *meta, meta_file_t *meta_file)
{
	if (!meta || !meta_file) 
		return -1;

	char8 *p  = NULL;
	int32 len = 0;

	if (p = strstr(meta, META_FILE_ANNOUNCE_TYPE))
	{
		p += strlen(META_FILE_ANNOUNCE_TYPE);

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');	
			p++;
		}

		// skip ':'
		p++;

		// get tracker url
		meta_file->tracker_num = 1;
		meta_file->tracker_url = (meta_file_t*)calloc(1, sizeof(meta_file_t));	
		memcpy(meta_file->tracker_url, p, len);
	}

	//FIXME multiple trackers
	
	return p ? 0 : -1;
}

int32 parse_metafile(const char8 *filepath, meta_file_t *meta_file)
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

	return parse_metafile2(meta, filesize, meta_file);
}

int32 parse_metafile2(const char8 *meta, int32 meta_size, meta_file_t *meta_file)
{
	if (!meta || !meta_file) return -1;

	memset(meta_file, 0, sizeof(meta_file_t));
	meta_file->meta_size = meta_size;

	// announce
	if (get_announce(meta, meta_file) != 0)
	{
		printf("get announce failed!\n");
		return -1;
	}

	// piece length
	if (get_piece_length(meta, meta_file) != 0)
	{
		printf("get piece length failed!\n");
		return -1;
	}
	
	// pieces
	if (get_pieces(meta, meta_file) != 0)
	{
		printf("get pieces failed!\n");
		return -1;
	}

	// file
	if (get_file(meta, meta_file) != 0)
	{
		printf("get file failed!\n");
		return -1;
	}

	// info hash
	if (get_info_hash(meta, meta_file) != 0)
	{
		printf("get info hash failed!\n");
		return -1;
	}

	// peer id
	if (get_peer_id(meta, meta_file) != 0)
	{
		printf("get peer id failed!\n");
		return -1;
	}

#if 1
	printf("=============metafile info===============\n");
	printf("[metafile] tracker num = %d\n", meta_file->tracker_num);
	printf("[metafile] tracker url = %s\n", meta_file->tracker_url);
	printf("[metafile] info hash = %s\n", meta_file->info_hash);
	printf("[metafile] peer id = %s\n", meta_file->peer_id);
	printf("[metafile] piece length = %d\n", meta_file->piece_length);
	printf("[metafile] pieces = %s\n", meta_file->pieces);
	printf("[metafile] is multi file = %d\n", meta_file->is_multi_file);
	printf("[metafile] floder name = %s\n", meta_file->floder_name);

	meta_file_file_t *file = meta_file->file;
	while (file)
	{
		printf("[metafile] file name = %s\n", file->file_name);
		printf("[metafile] file len = %llu\n", file->file_len);
		file = file->next;
	}
#endif

	return 0;
}
