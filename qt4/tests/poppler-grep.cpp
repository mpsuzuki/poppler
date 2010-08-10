#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QTextCodec>

#include <iostream>

#include <poppler-qt4.h>

int main( int argc, char **argv )
{
    int q_x = 0, q_y = 0;
    const char* keyword = NULL;
    const char* pdf_path = NULL;


    QCoreApplication a( argc, argv );               // QApplication required!

    {
      int i = 0;
      for ( i = 1; i < argc; i ++ )
      {
        switch( argv[i][0] ) {
        case '-':
          switch( argv[i][1] ) {
          case 'q':
          case 'x':
          case 'y':
            i++;
            if ( i >= argc )
              qWarning() << "option: -" << argv[i-1][1] << " requires a numerical argument";
            else if ( argv[i-1][1] == 'x' && argv[i-1][2] == 0 )
              q_x = atoi( argv[i] ); 
            else if ( argv[i-1][1] == 'y' && argv[i-1][2] == 0 )
              q_y = atoi( argv[i] ); 
            else if ( argv[i-1][1] == 'q' && argv[i-1][2] == 0 )
              q_x = q_y = atoi( argv[i] ); 
            break;
          default:
            qWarning() << "ivalid option: -" << argv[i][1] << " is ignored";
            break;
          }
          break;
        default:
          if ( !keyword )
            keyword = argv[i];
          else if ( !pdf_path )
            pdf_path = argv[i];
          else
	    qWarning() << "too many argument: " << argv[i] << " is ignored";
          break;
        }
      }
      std::cout << q_x << "," << q_y << "," << keyword << "," << pdf_path << std::endl;
    }

    if ( q_x < 0 || q_y < 0 || !keyword || !pdf_path )
    {
	qWarning() << "usage: poppler-grep -q <n> <keyword> <filename>";
	qWarning() << "     : poppler-grep -q_x <n_x> -q_y <n_y> <keyword> <filename>";
	exit(1);
    }
  
    Poppler::Document *doc = Poppler::Document::load( pdf_path);
    if (!doc)
    {
	qWarning() << "failed to load " << pdf_path;
	exit(1);
    }

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QString qstr_key = QString::fromLocal8Bit( keyword );
    QByteArray mbcs = qstr_key.toLocal8Bit();

    std::cout << "*** given keyword: " << std::flush;
    {
      int j;
      for ( j = 0; j < mbcs.size(); j++ )
        std::cout << mbcs[j];
      std::cout << std::endl;
      std::cout << std::flush;
    }

    int i;
    for ( i = 0; i < doc->numPages(); i++ )
    {
      int w, h;
      int x, y;
      int j = 0;

      Poppler::Page *page = doc->page(i);
      w = page->pageSizeF().width();
      h = page->pageSizeF().height();
      for ( x = 0; x < q_x ; x ++ ) 
      {
        for ( y = 0; y < q_y ; y ++ )
        {
          QRectF  rect = QRectF( (double)(x*w/q_x), (double)(y*h/q_y), (double)(w/q_x), (double)(h/q_y) );
          if ( page->search( qstr_key,
                             rect,
                             Poppler::Page::FromTop,
                             Poppler::Page::CaseInsensitive,
                             Poppler::Page::Rotate0,
                             true ) )
          {
            std::cout << "*** Page " << i << ", section (" << x << "," << y << ")" << std::endl;
#if 0
            std::cout << "         (" << x*w/q << "," << y*h/q << ") - (" << (x+1)*w/q << "," << (y+1)*h/q << ")" << std::endl;
            std::cout << std::flush;
            rect = QRectF( (double)(x*w/q_x), (double)(y*h/q_y), (double)(w/q_x), (double)(h/q_y) );
#else
            std::cout << "         (" << rect.toRect().bottomLeft().x();
            std::cout << ","          << rect.toRect().bottomLeft().y();
            std::cout << ") - ("      << rect.toRect().topRight().x();
            std::cout << ","          << rect.toRect().topRight().y();
            std::cout << ")" << std::endl;
#endif
            rect = QRectF( (double)(x*w/q_x), (double)(y*h/q_y), (double)(w/q_x), (double)(h/q_y) );
            mbcs = page->text( rect, TRUE ).toLocal8Bit();
            for ( j = 0; j < mbcs.size(); j++ )
              std::cout << mbcs[j];
            std::cout << std::endl;
            std::cout << std::flush;
          }
        }
      }
    }
    delete doc;
}
