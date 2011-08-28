extern char* stralloc_cat2( char* str1, char* str2 );

extern char* stralloc_cat3( char* str1, char* str2, char* str3 );

extern int init_readline_history(char*   history_file_prefix,
                                 char**  history_filename);
 
extern char* read_command_from_readline(char*    prompt,
					char*    history_filename,
                                        int*     toknum_p,
                                        char***  tok_p );
