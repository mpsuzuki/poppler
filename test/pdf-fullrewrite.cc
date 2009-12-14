//========================================================================
//
// pdf-fullrewrite.cc
//
// Copyright 2007 Julien Rebetez
//
//========================================================================
#include "config.h"
#include <poppler-config.h>
#include "GlobalParams.h"
#include "Error.h"
#include "PDFDoc.h"
#include "goo/GooString.h"
#include <cairo-pdf.h>
#include "CairoOutputDev.h"

int main (int argc, char *argv[])
{
  PDFDoc *doc;
  GooString *inputName, *outputName;

  // parse args
  if (argc < 3) {
    fprintf(stderr, "usage: %s INPUT-FILE OUTPUT-FILE\n", argv[0]);
    return 1;
  }

  inputName = new GooString(argv[1]);
  outputName = new GooString(argv[2]);

  globalParams = new GlobalParams();

  doc = new PDFDoc(inputName);

  if (!doc->isOk()) {
    delete doc;
    fprintf(stderr, "Error loading document !\n");
    return 1;
  }

  int x = doc->getPageMediaWidth(1)  / 2;
  int y = doc->getPageMediaHeight(1) / 2;
  int w = doc->getPageMediaWidth(1)  / 10;
  int h = doc->getPageMediaHeight(1) / 10;

  CairoOutputDev *cairoOut = new CairoOutputDev;
  {
    cairo_surface_t *surface = cairo_pdf_surface_create( argv[2], doc->getPageMediaWidth(1), doc->getPageMediaHeight(1) );
    cairo_t* cr = cairo_create( surface );
    cairo_surface_destroy( surface );
    cairoOut->setCairo( cr );
    cairo_destroy( cr );
  }
  cairoOut->startDoc( doc->getXRef(), doc->getCatalog() );

  {
    int page;

#if 0
    for ( page = 2; page < 10 && page < ( doc->getNumPages() + 1 ); page++ )
    {
      // cairoOut->startPage( page, NULL );
      // doc->displayPageSlice( cairoOut, page, 72, 72, 0, gFalse, gTrue, gTrue, x, y, w, h );
      doc->displayPage( cairoOut, page, 72, 72, 0, gFalse, gTrue, gTrue );
      // cairoOut->endPage();
    }
#else
    doc->displayPages( cairoOut, 1, 2, 72, 72, 0, gFalse, gTrue, gTrue );
#endif
  }

  cairoOut->setCairo( NULL );

  delete cairoOut;
  delete doc;
  delete globalParams;
  delete outputName;
  return 0;
}
