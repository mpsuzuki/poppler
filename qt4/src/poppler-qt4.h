/* poppler-qt.h: qt interface to poppler
 * Copyright (C) 2005, Net Integration Technologies, Inc.
 * Copyright (C) 2005, Brad Hards <bradh@frogmouth.net>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __POPPLER_QT_H__
#define __POPPLER_QT_H__

#ifdef UNSTABLE_POPPLER_QT4

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

/**
   The Poppler Qt bindings
*/
namespace Poppler {

    class Document;

    class PageData;

    class TextBox {
    public:
      TextBox(const QString& text, const QRectF &bBox) :
	m_text(text), m_bBox(bBox) {};
      
      QString text() const { return m_text; };
      QRectF boundingBox() const { return m_bBox; };
	
    private:
	QString m_text;
	QRectF m_bBox;
    };


    /**
       Container class for information about a font within a PDF
       document
    */
    class FontInfo {
    public:
	enum Type {
	    unknown,
	    Type1,
	    Type1C,
	    Type3,
	    TrueType,
	    CIDType0,
	    CIDType0C,
	    CIDTrueType
	};
	
	/**
	   Create a new font information container
	*/
	FontInfo( const QString fontName, const bool isEmbedded,
		  const bool isSubset, Type type):
	    m_fontName(fontName),
	    m_isEmbedded(isEmbedded),
	    m_isSubset(isSubset),
	    m_type(type)
	    {};

	/**
	   The name of the font. Can be QString::null if the font has no name
	*/
	QString name() const
	    { return m_fontName; }

	/**
	   Whether the font is embedded in the file, or not

	   \return true if the font is embedded
	*/
	bool isEmbedded() const
	    { return m_isEmbedded; }

	/**
	   Whether the font provided is only a subset of the full
	   font or not. This only has meaning if the font is embedded.

	   \return true if the font is only a subset
	*/
	bool isSubset() const
	    { return m_isSubset; }

	/**
	   The type of font encoding
	*/
	Type type() const
	    { return m_type; }

	QString typeName() const;
    private:
	QString m_fontName;
	bool m_isEmbedded;
	bool m_isSubset;
	Type m_type;
    };


    /**
       Page within a PDF document
    */
    class Page {
	friend class Document;
    public:
	~Page();

	/**
	   Render the page to a pixmap using the Splash renderer
	*/
	void splashRenderToPixmap(QPixmap **q, int x, int y, int w, int h) const;

	/**
	   Render the page to a pixmap using the Arthur (Qt4) renderer

	   \param q pointer to a QPixmap that is already set to the
	   intended size.

	   You are meant to create the pixmap before passing it to
	   this routine, using something like:
	   \code
	   QPixmap* myPixmap = new QPixmap(page->pageSize());
	   page->renderToPixmap(myPixmap);
	   \endcode
	*/
        void renderToPixmap(QPixmap *q, double xres, double yres) const;

        /**
         * This is a convenience function that is equivalent to
	 * renderToPixmap() with xres and yres set to 72.0. We keep it
	 * only for binary compatibility
	 **/
	void renderToPixmap(QPixmap *q) const;

	/**
	   Returns the text that is inside a specified rectangle

	   \param rect the rectangle specifying the area of interest
	   If rect is null, all text on the page is given
	**/
	QString text(const QRectF &rect) const;

	QList<TextBox*> textList() const;

	/**
	   The dimensions of the page, in points.
	*/
	QSizeF pageSizeF() const;

	/**
	   The dimensions of the page, in points.
	*/
	QSize pageSize() const;

	/**
	   Types of orientations that are possible
	*/
	enum Orientation {
	    Landscape, ///< Landscape orientation (portrait, with 90 degrees clockwise rotation )
	    Portrait, ///< Normal portrait orientation
	    Seascape, ///< Seascape orientation (portrait, with 270 degrees clockwise rotation)
	    UpsideDown ///< Upside down orientation (portrait, with 180 degrees rotation)
	};

	/**
	   The orientation of the page
	*/
	Orientation orientation() const;
    private:
	Page(const Document *doc, int index);
	PageData *m_page;
    };

    class DocumentData;

/**
   PDF document

   A document potentially contains multiple Pages
*/
    class Document {
	friend class Page;
  
    public:
	/**
	   The page mode
	*/
	enum PageMode {
	    UseNone,     ///< No mode - neither document outline nor thumbnail images are visible
	    UseOutlines, ///< Document outline visible
	    UseThumbs,   ///< Thumbnail images visible
	    FullScreen,  ///< Fullscreen mode (no menubar, windows controls etc)
	    UseOC,       ///< Optional content group panel visible
	    UseAttach    ///< Attachments panel visible
	};
  
	/**
	   The page layout
	*/
	enum PageLayout {
	    NoLayout,   ///< Layout not specified
	    SinglePage, ///< Display a single page
	    OneColumn,  ///< Display a single column of pages
	    TwoColumnLeft, ///< Display the pages in two columns, with odd-numbered pages on the left
	    TwoColumnRight, ///< Display the pages in two columns, with odd-numbered pages on the right
	    TwoPageLeft, ///< Display the pages two at a time, with odd-numbered pages on the left
	    TwoPageRight, ///< Display the pages two at a time, with odd-numbered pages on the right
	};

	/**
	   Load the document from a file on disk

	   \param filePath the name (and path, if required) of the file to load
	*/
	static Document *Document::load(const QString & filePath,
					const QByteArray &ownerPassword=QByteArray(),
					const QByteArray &userPassword=QByteArray());
  
	/**
	   Get a specified page
     
	   Note that this follows the PDF standard of being zero based - if you
	   want the first page, then you need an index of zero.

	   \param index the page number index
	*/
	Page *page(int index) const{ return new Page(this, index); }

	/**
	   \overload

	   The intent is that you can pass in a label like "ix" and
	   get the page with that label (which might be in the table of
	   contents), or pass in "1" and get the page that the user
	   expects (which might not be the first page, if there is a
	   title page and a table of contents).

	   \param label the page label
	*/
	Page *page(QString label) const;

	/**
	   The number of pages in the document
	*/
	int numPages() const;
  
	/**
	   The type of mode that should be used by the application
	   when the document is opened. Note that while this is 
	   called page mode, it is really viewer application mode.
	*/
	PageMode pageMode() const;

	/**
	   The layout that pages should be shown in when the document
	   is first opened.  This basically describes how pages are
	   shown relative to each other.
	*/
	PageLayout pageLayout() const;

	/**
	   Provide the passwords required to unlock the document
	*/
	bool unlock(const QByteArray &ownerPassword, const QByteArray &userPassword);

	/**
	   Determine if the document is locked
	*/
	bool isLocked() const;

	/**
	   The date associated with the document

	   You would use this method with something like:
	   \code
	   QDateTime created = m_doc->date("CreationDate");
	   QDateTime modded = m_doc->date("ModDate");
	   \endcode

	   \param data the type of date that is required (such as CreationDate or ModDate)

	*/
	QDateTime date( const QString & data ) const;

	/**
	   Get specified information associated with the document

	   You would use this method with something like:
	   \code
	   QString title = m_doc->info("Title");
	   QString subject = m_doc->info("Subject");
	   \endcode

	   In addition to Title and Subject, other information that may be available
	   includes Author, Keywords, Creator and Producer.

	   \param data the information that is required
	*/
	QString info( const QString & data ) const;

	/**
	   Test if the document is encrypted
	*/
	bool isEncrypted() const;

	/**
	   Test if the document is linearised

	   In some cases, this is called "fast web view", since it
	   is mostly an optimisation for viewing over the Web.
	*/
	bool isLinearized() const;

	/**
	   Test if the permissions on the document allow it to be
	   printed
	*/
	bool okToPrint() const;

	/**
	   Test if the permissions on the document allow it to be
	   printed at high resolution
	*/
	bool okToPrintHighRes() const;

	/**
	   Test if the permissions on the document allow it to be
	   changed.

	   \note depending on the type of change, it may be more
	   appropriate to check other properties as well.
	*/
	bool okToChange() const;

	/**
	   Test if the permissions on the document allow the
	   contents to be copied / extracted
	*/
	bool okToCopy() const;

	/**
	   Test if the permissions on the document allow annotations
	   to be added or modified, and interactive form fields (including
	   signature fields) to be completed.
	*/
	bool okToAddNotes() const;

	/**
	   Test if the permissions on the document allow interactive
	   form fields (including signature fields) to be completed.

	   \note this can be true even if okToAddNotes() is false - this
	   means that only form completion is permitted.
	*/
	bool okToFillForm() const;

	/**
	   Test if the permissions on the document allow interactive
	   form fields (including signature fields) to be set, created and
	   modified
	*/
	bool okToCreateFormFields() const;

	/**
	   Test if the permissions on the document allow content extraction
	   (text and perhaps other content) for accessibility usage (eg for
	   a screen reader)
	*/
	bool okToExtractForAccessibility() const;

	/**
	   Test if the permissions on the document allow it to be
	   "assembled" - insertion, rotation and deletion of pages;
	   or creation of bookmarks and thumbnail images. This can
	   be true even if okToChange is false.
	*/
	bool okToAssemble() const;

	/**
	   The version of the PDF specification that the document
	   conforms to
	*/
	double pdfVersion() const;
  
	/**
	   The fonts within the PDF document.

	   \note this can take a very long time to run with a large
	   document. You may wish to use the call below if you have more
	   than say 20 pages
	*/
	QList<FontInfo> fonts() const;

	/**
	   \overload

	   \param numPages the number of pages to scan
	   \param fontList pointer to the list where the font information
	   should be placed

	   \return false if the end of the document has been reached
	*/
	bool scanForFonts( int numPages, QList<FontInfo> *fontList ) const; 

	Document::~Document();
  
    private:
	DocumentData *m_doc;
	Document::Document(DocumentData *dataA);
    };

}
#endif

#endif
