#ifndef __BITSC_PARSE_METAFILE_H__
#define __BITSC_PARSE_METAFILE_H__

#include "utils.h"

typedef struct _meta_file_file_t {

	char8						file_name[1024];

	uint64						file_len;				

	struct _meta_file_file_t 	*next;

} meta_file_file_t;


typedef struct _meta_file_t {

	int32						tracker_num;

	char8               		*tracker_url;	

	int32                       piece_length;

	char8                       *pieces;

	int32               		is_multi_file;

	char8                       floder_name[1024];

	meta_file_file_t    		*file;

} meta_file_t;

int32 parse_metafile(const char8 *filepath, meta_file_t *meta_file);

int32 parse_metafile2(const char8 *meta, meta_file_t *meta_file);

#endif
