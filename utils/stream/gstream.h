#ifndef _GSTREAM_H_
#define _GSTREAM_H_

typedef struct gstream {
	tb_byte_t*                  data;

	tb_size_t                   size;

	tb_size_t                   offset;

	tb_int_t (*open)()

}tb_gstream_t;

tb_ssize_t tb_gstream_read(tb_gstream_t *gst, tb_byte_t *data, tb_size_t size);
tb_size_t  tb_gstream_left(tb_gstream_t *gst);
tb_size_t  tb_gstream_seek(tb_gstream_t *gst, tb_size_t size, tb_int_t whence);
tb_size_t  tb_gstream_skip(tb_gstream_t *gst, tb_size_t size);

#endif
