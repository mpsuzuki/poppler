#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTextCodec>

#include <iostream>

#include <poppler-qt4.h>

int main( int argc, char **argv )
{
    QCoreApplication a( argc, argv );               // QApplication required!

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    if ( argc < 2 ||
        (argc == 3 && strcmp(argv[2], "-raw") != 0 ) ||
         argc > 3)
    {
	qWarning() << QString::fromAscii( "usage: poppler-texts filename [-raw]" ).toLocal8Bit();
	exit(1);
    }
  
    Poppler::Page::TextLayout layout =
      ( argc == 3 && strcmp(argv[2], "-raw") == 0 ) ?
        Poppler::Page::RawOrderLayout : Poppler::Page::PhysicalLayout ;

    Poppler::Document *doc = Poppler::Document::load(argv[1]);
    if (!doc)
    {
	qWarning() << QString::fromAscii( "doc not loaded" ).toLocal8Bit();
	exit(1);
    }

    int i;
    for ( i = 0; i < doc->numPages(); i++ )
    {
      QByteArray mbcs;

      mbcs = QString::fromAscii("*** Page ").toLocal8Bit();
      std::cout << std::flush;
      for ( int j = 0; j < mbcs.size(); j++ )
        std::cout << mbcs[j];
      std::cout << std::endl;

      mbcs = QString::number(i).toLocal8Bit();
      std::cout << std::flush;
      for ( int j = 0; j < mbcs.size(); j++ )
        std::cout << mbcs[j];
      std::cout << std::endl;

      Poppler::Page *page = doc->page(i);
      QRectF rect = QRectF( 0,
                            0,
                            (int)doc->page(i)->pageSizeF().width(),
                            (int)doc->page(i)->pageSizeF().height() );

      mbcs = page->text( rect, layout ).toLocal8Bit();
      std::cout << std::flush;
      for ( int j = 0; j < mbcs.size(); j++ )
        std::cout << mbcs[j];
      std::cout << std::endl;

    }
    delete doc;
}
