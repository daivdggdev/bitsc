#ifndef _BSTREAM_H_
#define _BSTREAM_H_

typedef struct __tb_bstream_t {
	tb_byte_t*       p;
	tb_size_t        b;
	tb_byte_t*       e;
	tb_size_t        n;

}tb_bstream_t;

tb_bstream_t* tb_bstream_attach(tb_bstream_t *bst, tb_byte_t *data, tb_size_t size);
tb_void_t     tb_bstream_sync(tb_bstream_t *bst);
tb_void_t     tb_bstream_skip(tb_bstream_t *bst, tb_size_t size);
tb_size_t     tb_bstream_left(tb_bstream_t *bst);
tb_size_t     tb_bstream_get_data(tb_bstream_t *bst, tb_type_t *data, tb_size_t size);
tb_size_t     tb_bstream_set_data(tb_bstream_t *bst, tb_type_t *data, tb_size_t size);

// bits
tb_size_t tb_bstream_get_u1(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u8(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u16_le(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u16_be(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u24_le(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u24_be(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u32_le(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u32_be(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u64_le(tb_bstream_t *bst);
tb_size_t tb_bstream_get_u64_be(tb_bstream_t *bst);

#endif
