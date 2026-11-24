#include "json.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *json_string(const char *value) {
    if(value == NULL) {
        return strdup("null");
    }

    // Worst case every byte becomes a six-character \u00XX escape, plus two quotes and
    // the terminating null.
    size_t length = strlen(value);
    char *result = malloc(length * 6 + 3);
    if(result == NULL) {
        return NULL;
    }

    char *out = result;
    *out++ = '"';
    for(const unsigned char *in = (const unsigned char *)value; *in != '\0'; in++) {
        switch(*in) {
            case '"':  *out++ = '\\'; *out++ = '"';  break;
            case '\\': *out++ = '\\'; *out++ = '\\'; break;
            case '\b': *out++ = '\\'; *out++ = 'b';  break;
            case '\f': *out++ = '\\'; *out++ = 'f';  break;
            case '\n': *out++ = '\\'; *out++ = 'n';  break;
            case '\r': *out++ = '\\'; *out++ = 'r';  break;
            case '\t': *out++ = '\\'; *out++ = 't';  break;
            default:
                if(*in < 0x20) {
                    out += sprintf(out, "\\u%04x", *in);
                } else {
                    // Everything else, including UTF-8 multibyte sequences, is valid
                    // inside a JSON string as it is.
                    *out++ = *in;
                }
        }
    }
    *out++ = '"';
    *out = '\0';
    return result;
}

char *format_alloc(const char *format, ...) {
    va_list args;

    va_start(args, format);
    int length = vsnprintf(NULL, 0, format, args);
    va_end(args);
    if(length < 0) {
        return NULL;
    }

    char *result = malloc((size_t)length + 1);
    if(result == NULL) {
        return NULL;
    }

    va_start(args, format);
    vsnprintf(result, (size_t)length + 1, format, args);
    va_end(args);
    return result;
}
