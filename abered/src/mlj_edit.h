extern unsigned char word_wrap;

#define set_word_wrap()		word_wrap = 1
#define clr_word_wrap()		word_wrap = 0

char *edit_string(char *buf, int create_mem);
void set_header(char *s);
int  abort_edit();
