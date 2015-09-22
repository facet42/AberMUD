
#define NEW(T)		(T *)malloc(sizeof(T))
#define COPY(S)		strcpy((char *)malloc(strlen(S) + 1), S)
#define NEWLINE(F)      while(getc(F) != '\n')
#define EQ(S,T)		(strcasecmp(S,T) == 0)
#define ABREV_EQ(S,T)	(strncasecmp(S,T,strlen(T)) == 0)
#define EMPTY(S)  	((S == NULL) || (S[0] == '\0'))
#define FREE(T)		{ free(T); T = NULL; }
