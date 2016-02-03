#ifndef __BASE64_H__
#define __BASE64_H__

typedef struct buffer_st {
  char *data;
  size_t length;
  char *ptr;
  int offset;
} base64buf;

void buffer_new(struct buffer_st *b);
void buffer_add(struct buffer_st *b, char c);
void buffer_delete(struct buffer_st *b);

void base64_encode(struct buffer_st *b, const char *source, size_t length);
void base64_decode(struct buffer_st *b, const char *source, size_t length);


#endif
