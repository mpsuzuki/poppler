/* 
 * Copyright (C) 2010 suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <poppler.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>


extern char *optarg;
extern int optind, optopt, opterr;



int main (int argc, char **argv)
{
  double            resolution = 72;
  double            xoffset    = 0;
  double            yoffset    = 0;
  double            width      = 0;
  double            height     = 0;
  int               firstpage  = 0;
  int               lastpage   = 0;
  gboolean          invertY    = FALSE;

  PopplerDocument  *document;
  PopplerRectangle  rect;
  PopplerRectangle *selection = &rect;
  GError           *error = NULL;


  {
    int  c;

    while ( -1 != ( c = getopt( argc, argv, ":r:x:y:W:H:f:l:I" ) ) )
    {
      switch( c )
      {
      case 'I': /* invert Y */
        invertY = TRUE;
        break;
      case 'f': /* first page */
        firstpage = atoi( optarg ) - 1;
        break;
      case 'l': /* first page */
        lastpage = atoi( optarg ) - 1;
        break;
      case 'r': /* resolution */
        resolution = atof( optarg );
        break;
      case 'x': /* x offset */
        xoffset = atof( optarg );
        break;
      case 'y': /* y offset */
        yoffset = atof( optarg );
        break;
      case 'W': /* width */
        width = atof( optarg );
        break;
      case 'H': /* height */
        height = atof( optarg );
        break;
      default: /* help */
        printf( "Usage (TBD)\n" );
        exit( 1 );
      }
    }

    rect.x1 = xoffset * 72 / resolution;
    rect.y1 = yoffset * 72 / resolution;
    rect.x2 = ( xoffset + width )  * 72 / resolution;
    rect.y2 = ( yoffset + height ) * 72 / resolution;

    if ( 0 != access( argv[optind], R_OK ) )
    {
      fprintf( stderr, "Could not open %s\n", argv[optind] );
      exit( 2 );
    }
    else
    {
      char   *pathname = argv[optind];
      GFile  *file;
      char   *uri;


      if ( !g_thread_supported() )
        g_thread_init( NULL );

      gtk_init( &argc, &argv );

      if ( g_path_is_absolute( pathname ) ) {
        uri = g_filename_to_uri( pathname, NULL, &error );
      } else if ( g_ascii_strncasecmp( pathname, "file://", strlen( "file://" ) ) == 0 ) {
        uri = g_strdup ( pathname );
      } else if ( !g_strrstr( pathname, "://" ) ) {
        gchar *dir;
        gchar *filename;

        dir = g_get_current_dir( );
        filename = g_build_filename( dir, pathname, NULL );
        g_free (dir);
        uri = g_filename_to_uri (filename, NULL, &error);
        g_free (filename);
      } else {
        g_print ("Error: unsupported uri\n");
        exit( 3 );
      }

      if (error) {
        g_print ("Error: %s\n", error->message);
        g_error_free (error);
        exit( 4 );
      }

      document = poppler_document_new_from_file( uri, NULL, &error );
      if ( error )
      {
        g_print ("Error: %s\n", error->message);
        g_error_free( error );
        exit( 5 );
      }
      g_free( uri );
    }
  }


  {
    int  pg, maxpage;


    maxpage = poppler_document_get_n_pages( document );
    if ( maxpage < firstpage || lastpage < firstpage )
      exit( 6 );

    if ( maxpage < lastpage )
      lastpage = maxpage;

    for ( pg = firstpage; pg <= lastpage; pg++ )
    {
      PopplerPage*  page;
      char*         gottext;
      gdouble       page_width, page_height;


      page = poppler_document_get_page( document, pg );
      poppler_page_get_size( page, &(page_width), &(page_height) );
      if ( rect.x1 == 0 && rect.y1 == 0 && rect.x2 == 0 && rect.y2 == 0 )
      {
        rect.x2 = page_width;
        rect.y2 = page_height;
      }
      if ( invertY )
      {
        gdouble  orig_y1 = rect.y1;
        rect.y1 = ( page_height - rect.y2 );
        rect.y2 = ( page_height - orig_y1 );
      }
      gottext = poppler_page_get_text( page, POPPLER_SELECTION_GLYPH, selection );
      printf( "[Page %d]:[%s]\n", pg, gottext );
    }
  }
  exit( 0 );
}
