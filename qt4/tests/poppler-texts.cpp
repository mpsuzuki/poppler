#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTextCodec>

#include <iostream>

#include <poppler-qt4.h>

int main( int argc, char **argv )
{
    QCoreApplication a( argc, argv );               // QApplication required!

    if (!( argc == 2 ))
    {
	qWarning() << "usage: poppler-texts filename";
	exit(1);
    }
  
    Poppler::Document *doc = Poppler::Document::load(argv[1]);
    if (!doc)
    {
	qWarning() << "doc not loaded";
	exit(1);
    }

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    int i;
    for ( i = 0; i < doc->numPages(); i++ )
    {
      int j = 0;
      std::cout << "*** Page " << i << std::endl;
      std::cout << std::flush;

      Poppler::Page *page = doc->page(i);
      QRectF rect = QRectF( 0,
                            0,
                            (int)doc->page(i)->pageSizeF().width(),
                            (int)doc->page(i)->pageSizeF().height() );

      QByteArray mbcs = page->text( rect, TRUE ).toLocal8Bit();
      std::cout << std::flush;
      for ( j = 0; j < mbcs.size(); j++ )
        std::cout << mbcs[j];
      std::cout << std::endl;

    }
    delete doc;
}
