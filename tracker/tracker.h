#ifndef __BITSC_TRACKER_H__
#define __BITSC_TRACKER_H__

#include "metafile.h"

typedef struct _http_socket_t {

	int32 					sock;

	struct _http_socket_t   *next;

} http_socket_t;

typedef struct _peer_t {

	char8 					ip[16];

	int32 					port;

	struct _peer_t 			*next;

} peer_t;

typedef struct _tracker_t {

	http_socket_t 			*http_socket;

	peer_t 					*peer;

} tracker_t;

tracker_t *tracker_init();

int32 tracker_connect(tracker_t *tracker, meta_file_t *meta_file);

#endif
