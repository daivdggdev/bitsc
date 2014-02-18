#include "gstream.h"

tb_ssize_t tb_gstream_read(tb_gstream_t *gst, tb_byte_t *data, tb_size_t size)
{
	tb_assert_return_val(gst && data && szie > 0, -1);

	tb_size_t rsize = tb_min(gst->size - gst->offset, size);
	tb_assert_return_val(rsize > 0, -1);

	memcpy(data, gst->data + gst->offset, rsize);
	gst->offset += rsize;

	return rsize;
}

tb_size_t tb_gstream_left(tb_gstream_t *gst)
{
	tb_assert_return_val(gst && gst->offset < gst->size, 0);	

	return gst->size - gst->offset;	
}

tb_size_t tb_gstream_seek(tb_gstream_t *gst, tb_size_t size, tb_int_t whence)
{
	tb_assert_return(gst);	

	tb_size_t f = gst->offset;
	switch (whence)
	{
		case SEEK_SET: gst->offset = size; break;
		case SEEK_CUR: gst->offset += size; break;
		case SEEK_END: gst->offset = gst->size - size; break;
	}

	if (gst->offset > gst->size) 
		gst->offset = f;

	return gst->offset;
}

tb_size_t tb_gstream_skip(tb_gstream_t *gst, tb_size_t size)
{
	return tb_gstream_seek(gst, size, SEEK_CUR);
}
