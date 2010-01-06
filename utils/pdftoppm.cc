//========================================================================
//
// pdftoppm.cc
//
// Copyright 2003 Glyph & Cog, LLC
//
//========================================================================

//========================================================================
//
// Modified under the Poppler project - http://poppler.freedesktop.org
//
// All changes made under the Poppler project to this file are licensed
// under GPL version 2 or later
//
// Copyright (C) 2007 Ilmari Heikkinen <ilmari.heikkinen@gmail.com>
// Copyright (C) 2008 Richard Airlie <richard.airlie@maglabs.net>
// Copyright (C) 2009 Michael K. Johnson <a1237@danlj.org>
// Copyright (C) 2009 Shen Liang <shenzhuxi@gmail.com>
// Copyright (C) 2009 Stefan Thomas <thomas@eload24.com>
// Copyright (C) 2009, 2010 Albert Astals Cid <aacid@kde.org>
// Copyright (C) 2010 Adrian Johnson <ajohnson@redneon.com>
// Copyright (C) 2010 Hib Eris <hib@hiberis.nl>
// Copyright (C) 2010 Jonathan Liu <net147@gmail.com>
//
// To see a description of the changes please see the Changelog file that
// came with your tarball or type make ChangeLog if you are building from git
//
//========================================================================

#include "config.h"
#include <poppler-config.h>
#ifdef _WIN32
#include <fcntl.h> // for O_BINARY
#include <io.h>    // for setmode
#endif
#include <stdio.h>
#include <math.h>
#include "parseargs.h"
#include "goo/gmem.h"
#include "goo/GooString.h"
#include "GlobalParams.h"
#include "Object.h"
#include "PDFDoc.h"
#include "PDFDocFactory.h"
#include "splash/SplashBitmap.h"
#include "splash/Splash.h"
#include "SplashOutputDev.h"
#ifdef HAVE_CAIRO
#include "CairoOutputDev.h"
#include <cairo-pdf.h>
#include <cairo-svg.h>
#endif
#ifdef HAVE_READLINE
#define _FUNCTION_DEF /* avoid conflict between readline/rltypedefs.h & poppler/Function.h */
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#endif
#ifdef HAVE_LIBGEN_H
#include <libgen.h> /* POSIX.2 basename() */
#endif

#define PPM_FILE_SZ 512

static int firstPage = 1;
static int lastPage = 0;
static GBool printOnlyOdd = gFalse;
static GBool printOnlyEven = gFalse;
static double resolution = 0.0;
static double x_resolution = 150.0;
static double y_resolution = 150.0;
static int scaleTo = 0;
static int x_scaleTo = 0;
static int y_scaleTo = 0;
static int x = 0;
static int y = 0;
static int w = 0;
static int h = 0;
static int sz = 0;
static GBool useCropBox = gFalse;
static GBool mono = gFalse;
static GBool gray = gFalse;
static GBool png = gFalse;
static GBool jpeg = gFalse;
#ifdef HAVE_CAIRO
static GBool pdf = gFalse;
static GBool svg = gFalse;
#endif

#ifdef HAVE_CAIRO
#define  use_cairo  ( pdf || svg )
#define  use_multipage ( pdf )
#else
#define  use_cairo     gFalse
#define  use_multipage gFalse
#endif

#ifdef HAVE_READLINE
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
static GBool interactive = gFalse;
static const char* history_filename = NULL;
#endif
static char enableFreeTypeStr[16] = "";
static char antialiasStr[16] = "";
static char vectorAntialiasStr[16] = "";
static char ownerPassword[33] = "";
static char userPassword[33] = "";
static GBool quiet = gFalse;
static GBool printVersion = gFalse;
static GBool printHelp = gFalse;

static const ArgDesc argDesc[] = {
  {"-f",      argInt,      &firstPage,     0,
   "first page to print"},
  {"-l",      argInt,      &lastPage,      0,
   "last page to print"},
  {"-o",      argFlag,      &printOnlyOdd, 0,
   "print only odd pages"},
  {"-e",      argFlag,      &printOnlyEven, 0,
   "print only even pages"},

  {"-r",      argFP,       &resolution,    0,
   "resolution, in DPI (default is 150)"},
  {"-rx",      argFP,       &x_resolution,    0,
   "X resolution, in DPI (default is 150)"},
  {"-ry",      argFP,       &y_resolution,    0,
   "Y resolution, in DPI (default is 150)"},
  {"-scale-to",      argInt,       &scaleTo,    0,
   "scales each page to fit within scale-to*scale-to pixel box"},
  {"-scale-to-x",      argInt,       &x_scaleTo,    0,
   "scales each page horizontally to fit in scale-to-x pixels"},
  {"-scale-to-y",      argInt,       &y_scaleTo,    0,
   "scales each page vertically to fit in scale-to-y pixels"},

  {"-x",      argInt,      &x,             0,
   "x-coordinate of the crop area top left corner"},
  {"-y",      argInt,      &y,             0,
   "y-coordinate of the crop area top left corner"},
  {"-W",      argInt,      &w,             0,
   "width of crop area in pixels (default is 0)"},
  {"-H",      argInt,      &h,             0,
   "height of crop area in pixels (default is 0)"},
  {"-sz",     argInt,      &sz,            0,
   "size of crop square in pixels (sets W and H)"},
  {"-cropbox",argFlag,     &useCropBox,    0,
   "use the crop box rather than media box"},

  {"-mono",   argFlag,     &mono,          0,
   "generate a monochrome PBM file"},
  {"-gray",   argFlag,     &gray,          0,
   "generate a grayscale PGM file"},
#if ENABLE_LIBPNG
  {"-png",    argFlag,     &png,           0,
   "generate a PNG file"},
#endif
#if ENABLE_LIBJPEG
  {"-jpeg",    argFlag,     &jpeg,           0,
   "generate a JPEG file"},
#endif
#ifdef HAVE_CAIRO
  {"-pdf",    argFlag,     &pdf,           0,
   "generate a PDF file"},
  {"-svg",    argFlag,     &svg,           0,
   "generate a SVG file"},
#endif
#if HAVE_FREETYPE_FREETYPE_H | HAVE_FREETYPE_H
  {"-freetype",   argString,      enableFreeTypeStr, sizeof(enableFreeTypeStr),
   "enable FreeType font rasterizer: yes, no"},
#endif
#ifdef HAVE_READLINE
  {"-interactive", argFlag, &interactive, 0,
   "run in interactive mode"},
#endif
  
  {"-aa",         argString,      antialiasStr,   sizeof(antialiasStr),
   "enable font anti-aliasing: yes, no"},
  {"-aaVector",   argString,      vectorAntialiasStr, sizeof(vectorAntialiasStr),
   "enable vector anti-aliasing: yes, no"},
  
  {"-opw",    argString,   ownerPassword,  sizeof(ownerPassword),
   "owner password (for encrypted files)"},
  {"-upw",    argString,   userPassword,   sizeof(userPassword),
   "user password (for encrypted files)"},
  
  {"-q",      argFlag,     &quiet,         0,
   "don't print any messages or errors"},
  {"-v",      argFlag,     &printVersion,  0,
   "print copyright and version info"},
  {"-h",      argFlag,     &printHelp,     0,
   "print usage information"},
  {"-help",   argFlag,     &printHelp,     0,
   "print usage information"},
  {"--help",  argFlag,     &printHelp,     0,
   "print usage information"},
  {"-?",      argFlag,     &printHelp,     0,
   "print usage information"},
  {NULL}
};

static void savePageSlice(PDFDoc *doc,
                   SplashOutputDev *splashOut, 
                   int pg, int x, int y, int w, int h, 
                   double pg_w, double pg_h, 
                   char *ppmFile) {
  if (w == 0) w = (int)ceil(pg_w);
  if (h == 0) h = (int)ceil(pg_h);
  w = (x+w > pg_w ? (int)ceil(pg_w-x) : w);
  h = (y+h > pg_h ? (int)ceil(pg_h-y) : h);
  doc->displayPageSlice(splashOut, 
    pg, x_resolution, y_resolution, 
    0,
    !useCropBox, gFalse, gFalse,
    x, y, w, h
  );

  SplashBitmap *bitmap = splashOut->getBitmap();
  
  if (ppmFile != NULL) {
    if (png) {
      bitmap->writeImgFile(splashFormatPng, ppmFile, x_resolution, y_resolution);
    } else if (jpeg) {
      bitmap->writeImgFile(splashFormatJpeg, ppmFile, x_resolution, y_resolution);
    } else {
      bitmap->writePNMFile(ppmFile);
    }
  } else {
#if _WIN32
    setmode(fileno(stdout), O_BINARY);
#endif

    if (png) {
      bitmap->writeImgFile(splashFormatPng, stdout, x_resolution, y_resolution);
    } else if (jpeg) {
      bitmap->writeImgFile(splashFormatJpeg, stdout, x_resolution, y_resolution);
    } else {
      bitmap->writePNMFile(stdout);
    }
  }
}

static int numberOfCharacters(unsigned int n)
{
  int charNum = 0;
  while (n >= 10)
  {
    n = n / 10;
    charNum++;
  }
  charNum++;
  return charNum;
}

#ifdef HAVE_CAIRO
static void savePageSliceCairo(PDFDoc *doc,
                   CairoOutputDev *cairoOut,
                   int pg, int x, int y, int w, int h, 
                   double pg_w, double pg_h) {
  if (w == 0) w = (int)ceil(pg_w);
  if (h == 0) h = (int)ceil(pg_h);
  w = (x+w > pg_w ? (int)ceil(pg_w-x) : w);
  h = (y+h > pg_h ? (int)ceil(pg_h-y) : h);

  doc->displayPageSlice(cairoOut, pg, 72, 72, 0, !useCropBox, gFalse, gFalse, x, y, w, h);
}
#endif

#ifdef HAVE_READLINE

static char* read_command_from_readline(const char*  prompt,
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
    if ( line == NULL || 1 > strlen( line ) || line[0] == 3 || line[0] == 4 )
      break;

    add_history( line );
    if ( history_filename && 0 != append_history( 1, history_filename ) )
        fprintf( stderr, "Failed to append \"%s\" to %s\n", line, history_filename );

    cmdlen = strlen( line ) + 1;
    cmd = ( char * ) malloc( cmdlen );
    if ( cmd == NULL )
    {
      perror( "malloc" );
      *toknum_p = 0;
      *tok_p = NULL;
    }
    strncpy( cmd, line, cmdlen );
    if ( line != NULL )
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

      if ( *c != '\0' )
        goto find_a_delimiter;
    }
    free( line );


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
#endif

int main(int argc, char *argv[]) {
  PDFDoc *doc;
  GooString *fileName = NULL;
  char *ppmRoot = NULL;
  char ppmFile[PPM_FILE_SZ];
  GBool ppmFileIsNew;
  GooString *ownerPW, *userPW;
  SplashColor paperColor;
  SplashOutputDev *splashOut;
#ifdef HAVE_CAIRO
  cairo_surface_t *surface = NULL;
  cairo_t* cr = NULL;
  CairoOutputDev *cairoOut = NULL;
  GfxState  *state;
#endif
  int    toknum = argc;
  char** tok = argv;
  char*  rlbuff;
  const char*  prompt;

  GBool ok;
  int exitCode;
  int pg, pg_num_len;
  double pg_w, pg_h, tmp;

  exitCode = 99;
  memset( ppmFile,  0, sizeof(ppmFile) );

  // parse args
  ok = parseArgs(argDesc, &toknum, tok);
  if (!ok || toknum > 3 || printVersion || printHelp) {
    fprintf(stderr, "pdftoppm version %s\n", PACKAGE_VERSION);
    fprintf(stderr, "%s\n", popplerCopyright);
    fprintf(stderr, "%s\n", xpdfCopyright);
    if (!printVersion) {
      printUsage("pdftoppm", "[PDF-file [PPM-file-prefix]]", argDesc);
    }
    goto err0;
  }
  if (toknum > 1) fileName = new GooString(tok[1]);

#ifdef HAVE_READLINE
  /* read command string if no PDF pathname is given */
  if (interactive) {
    {
      size_t history_filename_buff_size = strlen( basename( tok[0] ) ) + strlen( "_history" ) + 2;
      char*  history_filename_buff = (char *)malloc( history_filename_buff_size );
      memset( history_filename_buff, 0, history_filename_buff_size );
      strcpy( history_filename_buff, "." );
      strcat( history_filename_buff, basename( tok[0] ) );
      strcat( history_filename_buff, "_history" );
      history_filename = (const char*)history_filename_buff;
      using_history();
      if ( 0 != read_history( history_filename ) )
      {
        fprintf( stderr, "Failed to read history file %s\n", history_filename );
        if ( 0 != write_history( history_filename ) )
          fprintf( stderr, "Failed to create history file %s\n", history_filename );
      }
      else if ( 1 != history_set_pos( history_length ) )
        fprintf( stderr, "History list is broken\n" );
    }

    {
      size_t prompt_buff_size = strlen( basename( tok[0] ) ) + 2;
      char*  prompt_buff = (char *)malloc( prompt_buff_size );
      memset( prompt_buff, 0, prompt_buff_size );
      strcpy( prompt_buff, basename( tok[0] ) );
      strcat( prompt_buff, ">" );
      prompt = (const char*)prompt_buff;
    }
    rlbuff = read_command_from_readline( prompt, &toknum, &tok );
    if (2 > toknum || !parseArgs(argDesc, &toknum, tok))
    {
      printUsage("pdftoppm", "[PDF-file [PPM-file-prefix]]", argDesc);
      goto err0;
    }
  }
#endif
  if (interactive && toknum > 1 ) /* in interactive mode, fileName is fixed */
    ppmRoot = tok[1];
  else if (!interactive && toknum > 2)
    ppmRoot = tok[2];
  else
    printUsage("pdftoppm", "[PDF-file [PPM-file-prefix]]", argDesc);

  // check exclusive options
  if (mono && gray) {
    fprintf(stderr, "mono and gray cannot be specified at once.\n");
    goto err0;
  }

  if ( 1 < ( ( png  ? 1 : 0 ) + ( jpeg ? 1 : 0 )
#ifdef HAVE_CAIRO
           + ( pdf  ? 1 : 0 ) + ( svg  ? 1 : 0 )
#endif
           ) ) {
    fprintf(stderr, "only one file format can be specified for output.\n");
    goto err0;
  }


  // read config file
  globalParams = new GlobalParams();
  if (enableFreeTypeStr[0]) {
    if (!globalParams->setEnableFreeType(enableFreeTypeStr)) {
      fprintf(stderr, "Bad '-freetype' value on command line\n");
    }
  }
  if (antialiasStr[0]) {
    if (!globalParams->setAntialias(antialiasStr)) {
      fprintf(stderr, "Bad '-aa' value on command line\n");
    }
  }
  if (vectorAntialiasStr[0]) {
    if (!globalParams->setVectorAntialias(vectorAntialiasStr)) {
      fprintf(stderr, "Bad '-aaVector' value on command line\n");
    }
  }
  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

  // open PDF file
  if (ownerPassword[0]) {
    ownerPW = new GooString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0]) {
    userPW = new GooString(userPassword);
  } else {
    userPW = NULL;
  }

  if (fileName == NULL) {
    fileName = new GooString("fd://0");
  }
  if (fileName->cmp("-") == 0) {
    delete fileName;
    fileName = new GooString("fd://0");
  }
  doc = PDFDocFactory().createPDFDoc(*fileName, ownerPW, userPW);
  delete fileName;

  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    exitCode = 1;
    goto err1;
  }

  // prepare GfxState buffer for CairoOutputDev
  if (use_cairo)
    state = new GfxState( 72 * w / x_resolution,
                          72 * h / y_resolution,
                          doc->getCatalog()->getPage(1)->getCropBox(),
                          doc->getCatalog()->getPage(1)->getRotate(),
                          gTrue );

process_a_command:
  // get page range
  if (firstPage < 1)
    firstPage = 1;
  if (lastPage < 1 || lastPage > doc->getNumPages())
    lastPage = doc->getNumPages();

  // set resolution parameters
  if ( resolution != 0.0 &&
       (x_resolution == 150.0 ||
        y_resolution == 150.0)) {
    x_resolution = resolution;
    y_resolution = resolution;
  }
  if (sz != 0) w = h = sz;
  pg_num_len = numberOfCharacters(doc->getNumPages());
  for (pg = firstPage; pg <= lastPage; ++pg) {
    if (printOnlyEven && pg % 2 == 0) continue;
    if (printOnlyOdd && pg % 2 == 1) continue;
    if (useCropBox) {
      pg_w = doc->getPageCropWidth(pg);
      pg_h = doc->getPageCropHeight(pg);
    } else {
      pg_w = doc->getPageMediaWidth(pg);
      pg_h = doc->getPageMediaHeight(pg);
    }

    if (scaleTo != 0) {
      resolution = (72.0 * scaleTo) / (pg_w > pg_h ? pg_w : pg_h);
      x_resolution = y_resolution = resolution;
    } else {
      if (x_scaleTo != 0) {
        x_resolution = (72.0 * x_scaleTo) / pg_w;
      }
      if (y_scaleTo != 0) {
        y_resolution = (72.0 * y_scaleTo) / pg_h;
      }
    }
    pg_w = pg_w * (x_resolution / 72.0);
    pg_h = pg_h * (y_resolution / 72.0);
    if ((doc->getPageRotate(pg) == 90) || (doc->getPageRotate(pg) == 270)) {
      tmp = pg_w;
      pg_w = pg_h;
      pg_h = tmp;
    }

    if (ppmRoot != NULL) {
      const char* suffix =
#ifdef HAVE_CAIRO
        pdf ? "pdf" : svg ? "svg" :
#endif
        png ? "png" : jpeg ? "jpg" : mono ? "pbm" : gray ? "pgm" : "ppm";

      ppmFileIsNew = gTrue;
      if (use_multipage && strlen( ppmFile ) )
        ppmFileIsNew = gFalse;
      else if (use_multipage)
        snprintf(ppmFile, PPM_FILE_SZ, "%.*s.%s", PPM_FILE_SZ - 32, ppmRoot, suffix );
      else
        snprintf(ppmFile, PPM_FILE_SZ, "%.*s-%0*d.%s", PPM_FILE_SZ - 32, ppmRoot, pg_num_len, pg, suffix );
    }


#ifdef HAVE_CAIRO
    if (use_cairo) {
      if (ppmFileIsNew) {
        if (surface)
          cairo_surface_destroy( surface );

        if (pdf)
          surface = cairo_pdf_surface_create( ppmFile,
                                              72 * w / x_resolution,
                                              72 * h / y_resolution );
        else if (svg)
          surface = cairo_svg_surface_create( ppmFile,
                                              72 * w / x_resolution,
                                              72 * h / y_resolution );
        cr = cairo_create( surface );
        cairoOut = new CairoOutputDev;
        cairoOut->setCairo( cr );
        cairo_destroy( cr );
        cairoOut->startDoc( doc->getXRef(), doc->getCatalog() );

      } else if (surface && use_multipage) {
        if (pdf)
          cairo_pdf_surface_set_size( surface,
                                      72 * w / x_resolution,
                                      72 * h / y_resolution );
      }

      cairoOut->saveState( state );
      savePageSliceCairo(doc, cairoOut, pg,
                         72 * x / x_resolution,
                         72 * y / y_resolution,
                         72 * w / x_resolution,
                         72 * h / y_resolution,
                         pg_w, pg_h);
      cairoOut->restoreState( state );
      if (!use_multipage) {
        cairoOut->setCairo( NULL );
        delete cairoOut;
      }
    }
#endif /* HAVE_CAIRO */
    if (!use_cairo) {
      if (ppmFileIsNew) {
        paperColor[0] = 255;
        paperColor[1] = 255;
        paperColor[2] = 255;
        splashOut = new SplashOutputDev(mono ? splashModeMono1 :
                                        gray ? splashModeMono8 :
                                               splashModeRGB8,
                                        4, gFalse, paperColor);
        splashOut->startDoc(doc->getXRef());
      }
      savePageSlice(doc, splashOut, pg, x, y, w, h, pg_w, pg_h, ppmFile);
      if (!use_multipage)
        delete splashOut;
    }
    ppmFileIsNew = gFalse;
  }

  if (!use_multipage) {
    if (use_cairo)
    {
      if (surface)
        cairo_surface_destroy( surface );
      cairoOut->setCairo( NULL );
      delete cairoOut;
    } else /* assume splashOut */
      delete splashOut;
  }

  if (interactive) {
    free( rlbuff );
    rlbuff = read_command_from_readline( prompt, &toknum, &tok );
    if (toknum > 1 && parseArgs(argDesc, &toknum, tok) ) {
      if (toknum == 1)
        ppmRoot = NULL;
      else { /* toknum > 2, output file is changed */
        ppmRoot = tok[1];
        memset( ppmFile,  0, sizeof(ppmFile) );
        if (use_cairo)
        {
          if (surface)
            cairo_surface_destroy( surface );
          cairoOut->setCairo( NULL );
          delete cairoOut;
        } else /* assume splashOut */
          delete splashOut;
      }
      goto process_a_command;
    }
    fprintf( stderr, "\nExit interactive mode\n" );
    free( rlbuff );
  }

  if (use_multipage) {
    if (use_cairo)
    {
      if (surface)
        cairo_surface_destroy( surface );
      cairoOut->setCairo( NULL );
      delete cairoOut;
    } else /* assume splashOut */
      delete splashOut;
  }

  exitCode = 0;

  // clean up
  if ( prompt )
    free( (char *)prompt );
  if ( history_filename )
    free( (char*)history_filename );
 err1:
  delete doc;
  delete globalParams;
 err0:

  // check for memory leaks
  Object::memCheck(stderr);
  gMemReport(stderr);

  return exitCode;
}
