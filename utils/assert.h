#ifndef _ASSERT_H_
#define _ASSERT_H_

#define tb_assert_return(p)                   do { if (!p) return} while (0);
#define tb_assert_return_val(p, x)            do { if (!p) return x; } while (0);

#define tb_assert_goto(p, x)                  do { if (!p) goto x; } while (0);
