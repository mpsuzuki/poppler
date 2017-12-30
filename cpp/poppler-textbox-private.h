#ifndef POPPLER_TEXTBOX_PRIVATE_H
#define POPPLER_TEXTBOX_PRIVATE_H

#include "poppler-rectangle.h"
#include "poppler-textbox.h"

namespace poppler
{
  class TextBox;

  class TextBoxData
  {
    public:
      TextBoxData()
        : nextWord(0), hasSpaceAfter(false)
      {
      }
      ustring text;
      rectf bBox;
      TextBox *nextWord;
      std::vector<rectf> charBBoxes;
      bool hasSpaceAfter;
  };
};

#endif
