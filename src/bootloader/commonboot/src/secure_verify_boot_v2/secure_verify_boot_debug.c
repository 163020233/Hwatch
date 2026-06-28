/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: secure verify boot debug.
 * Author: @CompanyNameTag
 * Create: 2024-01-11
*/

#ifdef CONFIG_SECURE_VERIFY_BOOT_DEBUG_ON
#include "boot_serial.h"
#include <stdarg.h>
#include <string.h>
#include "crypto_osal_lib.h"
#include "secure_verify_boot.h"

static char const hex2ascii_data[] = "0123456789abcdefgarhijklmnopqrstuvwxyz";
#define hex2ascii(hex) (hex2ascii_data[hex])
#define crypto_fallthrough() __attribute__((fallthrough))
#define toupper(c) ((c) - 0x20 * (((c) >= 'a') && ((c) <= 'z')))
static char *ksprintn(char *nbuf, unsigned int num, int base, int *lenp, int upper)
{
    char *p, c;
    p = nbuf;
    *p = '\0';
    do {
        c = hex2ascii(num % base);
        *++p = upper ? toupper(c) : c;
    } while (num /= base);
    if (lenp != NULL) {
        *lenp = p - nbuf;
    }
    return (p);
}
static int kvprintf(char const * fmt, void (*func)(char), va_list ap)
{
#define crypto_pchar(c) do {                         \
        char cc = (c);      \
        if (func != NULL)             \
            (*func)(cc); \
        retval++;             \
} while (0)
    char nbuf[sizeof(int) * 8 + 1];   // 8: 8 bits in one byte
    const char *p, *percent;
    int ch, n;
    unsigned int num;
    int base, tmp, width, neg, sign, dot;
    int dwidth, upper;
    char padc;
    int stop = 0;
    int retval = 0;
    num = 0;
    if (fmt == NULL) {
        fmt = "(fmt null)n";
    }
    for (;;) {
        padc = ' ';
        width = 0;
        while ((ch = (unsigned char)*fmt++) != '%' || stop) {
            if (ch == '\0') {
                return (retval);
            }
            crypto_pchar(ch);
        }
        percent = fmt - 1;
        neg = 0;
        sign = 0;
        dot = 0;
        dwidth = 0;
        upper = 0;
    reswitch:
        switch (ch = (unsigned char)*fmt++) {
            case '%':
                crypto_pchar(ch);
                break;
            case '0':
                if (!dot) {
                    padc = '0';
                    goto reswitch;
                }
                crypto_fallthrough();
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                for (n = 0;; ++fmt) {
                    n = n * 10 + ch - '0';  /* 10: Decimal data conversion. */
                    ch = *fmt;
                    if (ch < '0' || ch > '9') {
                        break;
                    }
                }
                if (dot) {
                    dwidth = n;
                } else {
                    width = n;
                }
                goto reswitch;
            case 'l': // Must use l flag to use u32
                goto reswitch;
            case 'c':
                crypto_pchar(va_arg(ap, int));
                break;
            case 'd':
            case 'i':
                base = 10;  /* 10: Decimal data. */
                sign = 1;
                goto handle_sign;
            case 'u':
                base = 10;  /* 10: Decimal data. */
                goto handle_nosign;
            case 's':
                p = va_arg(ap, char *);
                if (p == NULL) {
                    p = "(null)";
                }
                if (!dot) {
                    n = strlen(p);
                } else {
                    for (n = 0; n < dwidth && p[n]; n++) {
                        continue;
                    }
                }
                width -= n;
                if (width > 0) {
                    while (width--) {
                        crypto_pchar(padc);
                    }
                }
                while (n--) {
                    crypto_pchar(*p++);
                }
                break;
            case 'X':
                upper = 1;
                crypto_fallthrough();
            case 'x':
                base = 16;  /* 16: Hexadecimal data. */
                goto handle_nosign;
            handle_nosign:
                sign = 0;
                num = va_arg(ap, unsigned int);
                goto number;
            handle_sign:
                num = va_arg(ap, int);
            number:
                if (sign && (int)num < 0) {
                    neg = 1;
                    num = -(int)num;
                }
                p = ksprintn(nbuf, num, base, &tmp, upper);
                if (neg) {
                    tmp++;
                }
                if (padc != '0' && width && (width -= tmp) > 0) {
                    while (width--) {
                        crypto_pchar(padc);
                    }
                }
                if (neg) {
                    crypto_pchar('-');
                }
                if (width && (width -= tmp) > 0) {
                    while (width--) {
                        crypto_pchar(padc);
                    }
                }
                while (*p) {
                    crypto_pchar(*p--);
                }
                break;
            default:
                while (percent < fmt) {
                    crypto_pchar(*percent++);
                }
                /*
                 * Since we ignore an formatting argument it is no
                 * longer safe to obey the remaining formatting
                 * arguments as the arguments will no longer match
                 * the format specs.
                 */
                stop = 1;
                break;
        }
    }
#undef crypto_pchar
    return 0;
}
static void debug_putc(const char c)
{
    (void)serial_putc(c);
}

void secure_verify_boot_debug_messege(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    kvprintf(fmt, debug_putc, ap);
    va_end(ap);
}

/* crypto dump functions. */
#define MAX_DUMP_LENGHT     (512)
#define BYTES_IN_ONE_LINE   (16)
#ifndef KERN_CONT
#define KERN_CONT
#endif
void secure_verify_dump_data(const char *name, const uint8_t *data, uint32_t data_len)
{
    uint32_t i;
    uint32_t dump_length = data_len;
    crypto_unused(name);
    crypto_unused(data);
    if (data == NULL) {
        secure_verify_boot_print("dump data is NULL\n");
        return;
    }

    secure_verify_boot_print("%s(addr is 0x%lx, size is %d Bytes):\r\n", name, (uintptr_t)data, data_len);
    if (dump_length > MAX_DUMP_LENGHT) {
        dump_length = MAX_DUMP_LENGHT;
    }
    for (i = 0; i < dump_length; i++) {
        secure_verify_boot_print(KERN_CONT"%02x ", data[i]);
        if ((i + 1) % BYTES_IN_ONE_LINE == 0) {
            secure_verify_boot_print("\r\n");
        }
    }
    if (dump_length % BYTES_IN_ONE_LINE != 0) {
        secure_verify_boot_print("\r\n");
    }
}
#endif