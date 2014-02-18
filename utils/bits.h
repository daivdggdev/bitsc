#ifndef _BITS_H_
#define _BITS_H_

// 1 bit
#define tb_bits_get_u1(p)                ((*(p) >> 7) & 0x01) 

#define tb_bits_set_u1(p, x)             do { *(p) &= 0x7f; *(p) |= ((x & 0x01) << 7) } while (0);

// 8 bits
#define tb_bits_get_u8(p)                (*(p)) 

#define tb_bits_set_u8(p, x)             do { *(p) = (x); } while(0);           

// 16 bits
#define tb_bits_get_u16_le(p)            ( *(p + 1) << 8 | *(p) )
#define tb_bits_get_u16_be(p)            ( *(p) << 8 | *(p + 1) )
#define tb_bits_get_s16_le(p)            tb_bits_get_u16_le(p)
#define tb_bits_get_s16_be(p)            tb_bits_get_u16_be(p)

#define tb_bits_set_u16_le(p, x)         do { *p = (x | 0x00ff); *(p + 1) = (x >> 8); } while (0);
#define tb_bits_set_u16_be(p, x)         do { *p = (x >> 8); *(p + 1) = (x | 0x00ff); } while (0);

// 24 bits
#define tb_bits_get_u24_le(p)            ( *(p + 2) << 16 | *(p + 1) << 8 | *(p) )
#define tb_bits_get_u24_be(p)            ( *(p) << 16 | *(p + 1) << 8 | *(p + 2) )

#define tb_bits_set_u24_le(p, x)         do { *p = (x | 0x00ff); *(p + 1) = (x >> 8); *(p + 2) = (x >> 16); } while (0);
#define tb_bits_set_u24_be(p, x)         do { *p = (x >> 16); *(p + 1) = (x >> 8); *(p + 2) = (x | 0x00ff); } while (0);

// 32 bits
#define tb_bits_get_u32_le(p)            ( *(p + 3) << 24 | *(p + 2) << 16 | *(p + 1) << 8 | *(p) )
#define tb_bits_get_u32_be(p)            ( *(p) << 24 | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3) )

#define tb_bits_set_u32_le(p, x)         do { *p = (x | 0x00ff); *(p + 1) = (x >> 8); *(p + 2) = (x >> 16); *(p + 3) = (x >> 24); } while (0);
#define tb_bits_set_u32_be(p, x)         do { *p = (x >> 24); *(p + 1) = (x >> 16); *(p + 2) = (x >> 8); *(p + 3) = (x | 0x00ff); } while (0);

// 64 bits
#define tb_bits_get_u64_le(p)            ( *(p + 7) << 56 | *(p + 6) << 48 | *(p + 5) << 40 | *(p + 4) << 32 | *(p + 3) << 24 | *(p + 2) << 16 | *(p + 1) << 8 | *(p) )
#define tb_bits_get_u64_be(p)            ( *(p) << 56 | *(p + 1) << 48 | *(p + 2) << 40 | *(p + 3) << 32 | *(p + 4) << 24 | *(p + 5) << 16 | *(p + 6) << 8 | *(p + 7) )

#define tb_bits_set_u64_le(p, x)         do { *p = (x | 0x00ff); *(p + 1) = (x >> 8); *(p + 2) = (x >> 16); *(p + 3) = (x >> 24); *(p + 4) = (x >> 32); *(p + 5) = (x >> 40); *(p + 6) = (x >> 48); *(p + 7) = (x >> 56); } while (0);
#define tb_bits_set_u64_be(p, x)         do { *p = (x >> 56); *(p + 1) = (x >> 48); *(p + 2) = (x >> 40); *(p + 3) = (x >> 32); *(p + 4) = (x >> 24); *(p + 5) = (x >> 16); *(p + 6) = (x >> 8); *(p + 7) = (x | 0x00ff); } while (0);

#endif
