#include "bstream.h"

tb_bstream_t* tb_bstream_attach(tb_bstream_t *bst, tb_byte_t *data, tb_size_t size)
{
	tb_assert_return_val(bst && data, NULL);

	bst->p = data;
	bst->b = 0;
	bst->e = data + size;
	bst->n = size;

	return bst;
}

tb_void_t tb_bstream_sync(tb_bstream_t *bst)
{
	tb_assert_return(bst);

	if (bst->b)
	{
		bst->p ++;
		bst->b = 0;
	}
}

tb_void_t tb_bstream_skip(tb_bstream_t *bst, tb_size_t size)
{
	tb_assert_return(bst && (bst->p + size) <= bst->e);	

	tb_bstream_sync(bst);
	bst->p += size;
}

tb_size_t tb_bstream_left(tb_bstream_t *bst)
{
	tb_assert_return_val(bst, 0);

	tb_bstream_sync(bst);
	return bst->e - bst->p;
}

// data
tb_size_t tb_bstream_get_data(tb_bstream_t *bst, tb_type_t *data, tb_size_t size)
{
	tb_assert_return_val(bst && data && size > 0, 0);

	tb_size_t r = tb_min(size, tb_bstream_left(bst));
	memcpy(data, bst->p, r);
	bst->p += r;
	return r;	
}

tb_size_t tb_bstream_set_data(tb_bstream_t *bst, tb_type_t *data, tb_size_t size)
{
	tb_assert_return_val(bst && data && size > 0, 0);
	tb_assert_return_val(size <= tb_bstream_left(bst), 0);

	memcpy(bst->p, data, size);
	bst->p += size;
	return size;
}

// bits
tb_size_t tb_bstream_get_u1(tb_bstream_t *bst)
{
	tb_size_t val = (bst->p >> (7 - bst->b)) & 0x01;
	bst->b ++;

	if (bst->b >= 8)
	{
		bst->p ++;
		bst->b = 0;
	}

	return val;
}

tb_size_t tb_bstream_get_u8(tb_bstream_t *bst)
{
	return *(bst->p ++);
}

tb_size_t tb_bstream_get_u16_le(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u16_le(bst->p);
	bst->p += 2;
	return val;
}

tb_size_t tb_bstream_get_u16_be(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u16_be(bst->p);
	bst->p += 2;
	return val;
}

tb_size_t tb_bstream_get_u24_le(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u24_le(bst->p);
	bst->p += 3;
	return val;
}

tb_size_t tb_bstream_get_u24_be(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u24_be(bst->p);
	bst->p += 3;
	return val;
}

tb_size_t tb_bstream_get_u32_le(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u32_le(bst->p);
	bst->p += 4;
	return val;
}

tb_size_t tb_bstream_get_u32_be(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u32_be(bst->p);
	bst->p += 4;
	return val;
}

tb_size_t tb_bstream_get_u64_le(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u64_le(bst->p);
	bst->p += 8;
	return val;
}

tb_size_t tb_bstream_get_u64_be(tb_bstream_t *bst)
{
	tb_size_t val = tb_bits_get_u64_be(bst->p);
	bst->p += 8;
	return val;
}
