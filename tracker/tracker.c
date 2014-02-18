#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include "tracker.h"
#include "bits.h"

#define TRACKER_CONNECT_PORT               6881

int32 http_encode(uchar8 *in, int32 len1, char8 *out, int32 len2)
{
	int  i, j;
	char hex_table[16] = "0123456789abcdef"; 
	
	if( (len1 != 20) || (len2 <= 90) )  return -1;
	for(i = 0, j = 0; i < 20; i++, j++) 
	{
		if( isalpha(in[i]) || isdigit(in[i]) )
			out[j] = in[i];
		else 
		{ 
			out[j] = '%';
			j++;
			out[j] = hex_table[in[i] >> 4];
			j++;
			out[j] = hex_table[in[i] & 0xf];
		}
	}
	out[j] = '\0';
	
	return 0;
}

int32 get_port_by_tracker(char8 *tracker_url)
{

}

int32 get_host_ip_by_tracker(char8 *tracker_url, char8 **host, int32 *port)
{
	if (!tracker_url)
		return -1;

	if (strncmp(tracker_url, "http://", 7) != 0)
	{
		printf("tracker_url not http!\n");
		return -1;
	}
	
	*port = 80;

	char8 *p = NULL;
	char8 *q = NULL;
	int32 len = 0;

	if ((p = strchr(tracker_url + 7, ':')))
	{
		q = p + 1;

		while (isdigit(*q))
		{
			len = len * 10 + (*q - '0');
			q++;
		}

		*port = len;
	}
	else
	{
		if (!(p = strchr(tracker_url + 7, '/')))
			return -1;
	}

	int32 size = p - (tracker_url + 7);
	*host = (char8*)calloc(1, size + 1);
	strncpy(*host, tracker_url + 7, size);
	printf("host = %s, port = %d\n", *host, *port);
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

	char8 info_hash_encode[128] = {0};
	char8 peer_id_encode[128] = {0};
	http_encode(meta_file->info_hash, 20, info_hash_encode, 128);
	http_encode(meta_file->peer_id, 20, peer_id_encode, 128);

	/*printf("info hash encode = %s\n", info_hash_encode);*/
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
	info_hash_encode, peer_id_encode, port, upload, down,
	left, numwant);
#endif

	printf("request = %s\n", request);
	return 0;
}

int32 socket_connect(int32 sock, struct sockaddr *remote)
{
	// set socket no block
	int32 flag = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flag | O_NONBLOCK);

	int32 ret = connect(sock, remote, sizeof(struct sockaddr));
	if (ret != 0)
	{
		if (errno == EINPROGRESS)
		{
			printf("get the connect result by select().\n");
			int32 retry = 5;
			while (retry--)
			{
				fd_set wfd, rfd;
				FD_ZERO(&wfd);
				FD_ZERO(&rfd);
				FD_SET(sock, &wfd);
				FD_SET(sock, &rfd);

				struct timeval tv;
				tv.tv_sec = 10;
				tv.tv_usec = 0;

				int32 selret = select(sock + 1, &rfd, &wfd, NULL, &tv);
				if (selret == -1)
				{
					printf("select error!\n");
					continue;
				}
				else if (selret == 0)
				{
					printf("select timeout!\n");
					continue;
				}

				if (FD_ISSET(sock, &rfd) || FD_ISSET(sock, &wfd))
				{
					connect(sock, remote, sizeof(struct sockaddr));
					int32 err = errno;
					if  (err == EISCONN)    
					{    
						printf("connect finished.\n");    
						ret = 0;
						break;
					}    
					else    
					{    
						printf("connect failed. errno = %d\n", errno);    
						printf("FD_ISSET(sock_fd, &rfds): %d\n FD_ISSET(sock_fd, &wfds): %d\n", FD_ISSET(sock, &rfd) , FD_ISSET(sock, &wfd));    
						continue;
					}
				}
			}
		}
		else
		{
			perror("socket connect failed!");
		}
	}

	fcntl(sock, F_SETFL, flag);

	return (ret == 0) ? 0 : -1;
}

int32 get_peer_info(tracker_t *tracker, char8 *tracker_response)
{
	if (!tracker || !tracker_response)	
		return -1;

	if (strstr(tracker_response, "failure reason"))	
		return -1;

	char8 *p = NULL;	
	int32 len = 0;
	if ((p = strstr(tracker_response, "5:peers")))
	{
		p += strlen("5:peers");	

		while (isdigit(*p))
		{
			len = len * 10 + (*p - '0');
			p++;
		}
		p++;

		// 每个peer占6个字节
		int32 peer_num = len / 6;

		/*peer_t *p    = NULL;*/
		/*peer_t *node = NULL;*/

		while (peer_num--)
		{
			/*node = (peer_t*)calloc(1, sizeof(peer_t));	*/
			/*strcpy(node->ip, p, );*/
			printf("peer ip = %d.%d.%d.%d\n", *(p++), *(p++), *(p++), *(p++));

			/*char8 port[2] = {0};*/
			/*strncpy(port, p, 2);*/
			/*printf("peer port = ");*/
		}
	}

	return 0;
}

int32 tracker_connect(tracker_t *tracker, meta_file_t *meta_file)
{
	if (!tracker || !meta_file)
		return -1;

	http_socket_t *p    = NULL;
	http_socket_t *node = NULL;

	meta_file_announce_t *announce = meta_file->announce;
	while (announce)
	{
		// create socket
		int32 sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1) continue;

		// get host
		char8 *host = NULL;
		int32 port = 0;
		if (get_host_ip_by_tracker(announce->url, &host, &port) != 0)
		{
			printf("get_host_by_tracker failed!\n");
			announce = announce->next;
			continue;
		}

		// get ip
		char8 ip[16] = {0};	
		if (get_ip_by_host(host, ip) != 0)
		{
			printf("get_ip_by_host failed!\n");
			announce = announce->next;
			continue;
		}

		// config remote
		struct sockaddr_in remote;
		remote.sin_family = AF_INET;
		remote.sin_port = htons(port);
		if (inet_pton(AF_INET, ip, (void*)(&(remote.sin_addr.s_addr))) != 1)
		{
			printf("inet_pton failed!\n");
			announce = announce->next;
			continue;
		}

		// connect
		if (socket_connect(sock, (struct sockaddr*)&remote) != 0)
		/*if (connect(sock, (struct sockaddr*)&remote, sizeof(struct sockaddr)) != 0)*/
		{
			announce = announce->next;
			continue;
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
			announce = announce->next;
			continue;
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
			get_peer_info(tracker, recv_buf);
		}

		FREE(host);

		// save vaild socket
		node = (http_socket_t*)calloc(1, sizeof(http_socket_t));
		node->sock = sock;

		if (!tracker->http_socket)
		{
			tracker->http_socket = node;
		}
		else
		{
			p = tracker->http_socket;
			while (p->next) p = p->next;
			p->next = node;
		}

		announce = announce->next;
	}

	return 0;
}

tracker_t *tracker_init()
{
	tracker_t *tracker = (tracker_t*)calloc(1, sizeof(tracker_t));
	return tracker;
}
