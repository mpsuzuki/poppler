#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <iostream>

#include <poppler-qt4.h>

int main( int argc, char **argv )
{
    QCoreApplication a( argc, argv );               // QApplication required!

    if (!( argc == 5 ))
    {
	qWarning() << "usage: poppler-grep -q <n> <string> <filename>";
	exit(1);
    }
  
    Poppler::Document *doc = Poppler::Document::load(argv[4]);
    if (!doc)
    {
	qWarning() << "doc not loaded";
	exit(1);
    }

    int i;
    for ( i = 0; i < doc->numPages(); i++ )
    {
      int w, h;
      int q = atoi( argv[2] );
      int x, y;
      int j = 0;

      Poppler::Page *page = doc->page(i);
      w = doc->page(i)->pageSizeF().width();
      h = doc->page(i)->pageSizeF().height();
      for ( x = 0; x < q ; x ++ ) 
      {
        for ( y = 0; y < q ; y ++ )
        {
          QRectF  rect = QRectF( x*w/q, y*h/q, (x+1)*w/q, (y+1)*h/q );
          if ( page->search( argv[3], rect, Poppler::Page::FromTop, Poppler::Page::CaseInsensitive, Poppler::Page::Rotate0, true ) )
          {
            std::cout << "*** Page " << i << ", section (" << x << "," << y << ")" << std::endl;
            std::cout << std::flush;

            QByteArray utf8str = page->text( rect, TRUE ).toUtf8();
            for ( j = 0; j < utf8str.size(); j++ )
              std::cout << utf8str[j];
            std::cout << std::endl;
            std::cout << std::flush;
          }
        }
      }
    }
    delete doc;
}
