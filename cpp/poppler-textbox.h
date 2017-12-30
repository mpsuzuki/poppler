#ifndef POPPLER_TEXTBOX_H
#define POPPLER_TEXTBOX_H

#include "poppler-rectangle.h"

namespace poppler {
  class TextBoxData;

  class POPPLER_CPP_EXPORT TextBox {
    friend class page;
    public:
    TextBox(const ustring &text, const rectf &bBox);
    ~TextBox();
    ustring text() const;
    rectf boundingBox() const;
    TextBox *nextWord() const;
    rectf charBoundingBox(int i) const;
    bool hasSpaceAfter() const;
  private:
    TextBoxData* m_data;
  };
}

#endif
