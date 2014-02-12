#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include "tracker.h"

#define TRACKER_CONNECT_PORT               6881

int32 get_host_by_tracker(char8 *tracker_url, char8 **host)
{
	if (!tracker_url)
		return -1;

	if (strncmp(tracker_url, "http://", 7) != 0)
	{
		printf("tracker_url not http!\n");
		return -1;
	}
	
	char8 *p = NULL;
	if (!(p = strchr(tracker_url + 7, ':')))
	{
		if (!(p = strchr(tracker_url + 7, '/')))
			return -1;
	}

	int32 size = p - (tracker_url + 7);
	*host = (char8*)calloc(1, size + 1);
	strncpy(*host, tracker_url + 7, size);
	printf("host = %s\n", *host);
	return 0;
}

int32 get_ip_by_host(char8 *host, char8 ip[16])
{
	if (!host) return -1;

	struct hostent *hent;
	if (!(hent = gethostbyname(host)))
	{
		herror("gethostbyname failed!\n");
		return -1;
	}

	if (inet_ntop(AF_INET, (void*)hent->h_addr_list[0], ip, 15) == NULL)
	{
		perror("inet_ntop failed!\n");
		return -1;
	}

	ip[15] = '\0';
	printf("get ip = %s\n", ip);
	return 0;
}

int32 create_tracker_request(
		meta_file_t *meta_file, 
		char8 *host,
		uint32 port, 
		uint64 down,
		uint64 upload,
		uint64 left,
		int32  numwant,
		char8 *request)
{
	if (!meta_file || !host || !request)	
		return -1;

#if 0
	sprintf(request,
	"GET /announce?info_hash=%s&peer_id=%s&port=%u"
	"&uploaded=%lld&downloaded=%lld&left=%lld"
	"&event=started&compact=1&numwant=%d HTTP/1.0\r\n"
	"Host: %s\r\nUser-Agent: Bittorrent\r\nAccept: */*\r\n"
	"Accept-Encoding: gzip\r\nConnection: closed\r\n\r\n",
	meta_file->info_hash, meta_file->peer_id, port, upload, down,
	left, numwant, host);
#else
	sprintf(request,
	"GET /announce?info_hash=%s&peer_id=%s&port=%u"
	"&uploaded=%lld&downloaded=%lld&left=%lld"
	"&event=started&compact=1&numwant=%d HTTP/1.0\r\n"
	"User-Agent: Bittorrent\r\n\r\n",
	meta_file->info_hash, meta_file->peer_id, port, upload, down,
	left, numwant);
#endif

	printf("request = %s\n", request);
	return 0;
}

int32 tracker_connect(meta_file_t *meta_file)
{
	if (!meta_file)
		return -1;

	// create socket
	int32 sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) return -1;

	// get host
	char8 *host = NULL;
	if (get_host_by_tracker(meta_file->tracker->url, &host) != 0)
	{
		printf("get_host_by_tracker failed!\n");
		return -1;
	}

	// get ip
	char8 ip[16] = {0};	
	if (get_ip_by_host(host, ip) != 0)
	{
		printf("get_ip_by_host failed!\n");
		return -1;
	}

	// config remote
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(6969);
	if (inet_pton(AF_INET, ip, (void*)(&(remote.sin_addr.s_addr))) != 1)
	{
		printf("inet_pton failed!\n");
		return -1;
	}

#if 0
	// set socket no block
	int32 flags = fcntl(s, F_GETFL, 0);
	fcntl(s, F_SETFL, flags | O_NONBLOCK);
#endif

	// connect
	if (connect(sock, (struct sockaddr*)&remote, sizeof(struct sockaddr)) != 0)
	{
		perror("socket connect failed!");
		return -1;
	}

	// get left undownload size
	int64 left = 0;
	meta_file_file_t *file = meta_file->file;
	while (file)
	{
		left += file->file_len;
		file = file->next;
	}

	// create request 
	char8 request[1024] = {0};
	if (create_tracker_request(meta_file, host, TRACKER_CONNECT_PORT, 0, 0, left, 50, request) != 0)
	{
		printf("create_tracker_request failed!\n");
		return -1;
	}

	// send
	int32 sent = 0;
	int32 ret = 0;
	int32 need_send = strlen(request);
	while (sent < need_send)
	{
		ret = send(sock, request + sent, need_send - sent, 0);	
		if (ret == -1)
		{
			perror("socket send buf failed!\n");
			usleep(200000);
			continue;
		}

		sent += ret;
	}

	// recv
	char8 recv_buf[1024] = {0};
	while (recv(sock, recv_buf, 1024, 0) > 0)
	{
		printf("recv buf = %s\n", recv_buf);
	}

	FREE(host);
	return 0;
}
