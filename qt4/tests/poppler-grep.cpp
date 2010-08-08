#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTextCodec>
#include <iconv.h>

#include <iostream>

#include <poppler-qt4.h>

int main( int argc, char **argv )
{
    QCoreApplication a( argc, argv );               // QApplication required!

    if (!( argc == 5 )) /* XXX: more C++ or Qt4 manner */
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

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    // QTextCodec::setCodecForCStrings(QTextCodec::codecForName( "EUC-JP" ));
    // QTextCodec* codec = QTextCodec::codecForName( "EUC-JP" );
    QString qstr_key = QString::fromLocal8Bit( argv[3] );
    QByteArray qstr_utf8 = qstr_key.toUtf8();

    std::cout << "*** given keyword: " << std::flush;
    {
      int j;
      for ( j = 0; j < qstr_utf8.size(); j++ )
        std::cout << qstr_utf8[j];
      std::cout << std::endl;
      std::cout << std::flush;
    }

    int i;
    for ( i = 0; i < doc->numPages(); i++ )
    {
      int w, h;
      int q = atoi( argv[2] ); /* XXX: more C++ manner */
      int x, y;
      int j = 0;

      Poppler::Page *page = doc->page(i);
      w = page->pageSizeF().width();
      h = page->pageSizeF().height();
      for ( x = 0; x < q ; x ++ ) 
      {
        for ( y = 0; y < q ; y ++ )
        {
          QRectF  rect = QRectF( (double)(x*w/q), (double)(y*h/q), (double)(w/q), (double)(h/q) );
          if ( page->search( qstr_key,
                             rect,
                             Poppler::Page::FromTop,
                             Poppler::Page::CaseInsensitive,
                             Poppler::Page::Rotate0,
                             true ) )
          {
            std::cout << "*** Page " << i << ", section (" << x << "," << y << ")" << std::endl;
            std::cout << "         (" << x*w/q << "," << y*h/q << ") - (" << (x+1)*w/q << "," << (y+1)*h/q << ")" << std::endl;
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
