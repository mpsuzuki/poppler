#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#ifdef HAVE_LIBGEN_H
#include <libgen.h> /* POSIX.2 basename() */
#endif

#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )


char* stralloc_cat2(char* str1,
                    char* str2) {
  char* cat_str = (char*)malloc( strlen( str1 ) + strlen( str2 ) + 1 );
  if ( cat_str ) {
    strcpy( cat_str, str1 );
    strcat( cat_str, str2 );
  }
  return cat_str;
}

char* stralloc_cat3(char* str1,
                    char* str2,
                    char* str3) {
  char* cat_str = (char*)malloc( strlen( str1 ) + strlen( str2 ) + strlen( str3 ) + 1 );
  if ( cat_str ) {
    strcpy( cat_str, str1 );
    strcat( cat_str, str2 );
    strcat( cat_str, str3 );
  }
  return cat_str;
}

int init_readline_history(char*   history_file_prefix,
                          char**  history_filename) {
  *history_filename = stralloc_cat3( ".", history_file_prefix, "_history" );
  using_history();
  if ( 0 != read_history( *history_filename ) )
  {
    fprintf( stderr, "Failed to read history file %s\n", *history_filename );
    if ( 0 != write_history( *history_filename ) )
    {
      fprintf( stderr, "Failed to create history file %s\n", *history_filename );
      return -2;
    }
  }
  else if ( 1 != history_set_pos( history_length ) )
  {
    fprintf( stderr, "History list is broken\n" );
    return -1;
  }
  return 0;
}

 
char* read_command_from_readline(char*  prompt,
                                 char*  history_filename,
                                 int*         toknum_p,
                                 char***      tok_p ) {
  for (;;)
  {
    char*  line;
    char*  cmd;
    int    cmdlen;
    int    sepnum = 0;

    char** tok;
    int    toknum = 0;

    line = readline( prompt );

    if ( line == NULL )
      break;
    if ( !strlen( line ) || line[0] == 3 || line[0] == 4 )
    {
      free( line );
      break;
    }

    add_history( line );
    if ( history_filename && 0 != append_history( 1, history_filename ) )
        fprintf( stderr, "Failed to append \"%s\" to %s\n", line, history_filename );

    cmdlen = strlen( line ) + 1;
    cmd = ( char * ) malloc( cmdlen );
    if ( cmd == NULL )
    {
      perror( "malloc" );
      free( line );
      break;
    }
    strncpy( cmd, line, cmdlen );
    free( line );

    {
      char* cs = '\0';
      char* c  = cmd;
      char* c_sep;
      char* c_esc;

find_a_delimiter:
      c_sep = c + strlen( c ); /* addr points terminating NULL */

      cs = strchr( c, ' ' );
      if ( cs )
        c_sep = MIN( c_sep, cs );

      cs = strchr( c, '\t' );
      if ( cs )
        c_sep = MIN( c_sep, cs );

      cs = strchr( c, '\r' );
      if ( cs )
        c_sep = MIN( c_sep, cs );

      cs = strchr( c, '\n' );
      if ( cs )
        c_sep = MIN( c_sep, cs );

      cs = strchr( c, '\v' );
      if ( cs )
        c_sep = MIN( c_sep, cs );

      cs = strchr( c, '\f' );
      if ( cs )
        c_sep = MIN( c_sep, cs );

      do
      {
        c_esc = strchr( c, '\\' );
        if ( c_esc == NULL )
          ;
        else if ( ( c_esc + 1 ) == c_sep ) /* escaped delimiter. Jump to next delimiter */
        {
          c = c_esc + 2;
          goto find_a_delimiter;
        }
        else
          c_esc += 2; /* Skip escaped character */
      } while ( c_esc != NULL && c_esc < c_sep );

      *c_sep = '\0';
      c = c_sep + 1;
      sepnum ++;

      if ( c >= cmd + cmdlen )
        ;
      else if ( *c != '\0' )
        goto find_a_delimiter;
    }

    {
      char *c;
      int i;

      tok = (char **)malloc( sizeof( char* ) * ( sepnum + 1 ) );    
      tok[0] = (char *)prompt;
      for ( i = 0, c = cmd, toknum = 1; i < sepnum && c < ( cmd + cmdlen ); i++ )
      {
        if ( strlen( c ) > 0 )
        {
          tok[toknum] = c;
          toknum ++;
        }
        c += strlen( c ) + 1;
      }
    }

    {
      int i;
      fprintf( stderr, "\n" );
      for ( i = 0; i < toknum; i++ )
        fprintf( stderr, "token #%02d:'%s'\n", i, tok[i] );
    }

    *toknum_p = toknum;
    *tok_p = tok;
    return cmd;
  }

  *toknum_p = 0;
  *tok_p = NULL;
  return NULL;
}
