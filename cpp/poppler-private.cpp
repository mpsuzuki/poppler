/*
 * Copyright (C) 2009-2010, Pino Toscano <pino@kde.org>
 * Copyright (C) 2013 Adrian Johnson <ajohnson@redneon.com>
 * Copyright (C) 2014, Hans-Peter Deifel <hpdeifel@gmx.de>
 * Copyright (C) 2016 Jakub Alba <jakubalba@gmail.com>
 * Copyright (C) 2017, 2018 Albert Astals Cid <aacid@kde.org>
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
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "poppler-private.h"

#include "GooString.h"
#include "PDFDocEncoding.h"
#include "Page.h"

#include <ctime>
#include <iostream>
#include <sstream>

using namespace poppler;

static void stderr_debug_function(const std::string &msg, void * /*data*/)
{
    std::cerr << "poppler/" << msg;
}

debug_func detail::user_debug_function = stderr_debug_function;
void *detail::debug_closure = nullptr;

void detail::error_function(void * /*data*/, ErrorCategory /*category*/, Goffset pos, char *msg)
{
    std::ostringstream oss;
    if (pos >= 0) {
        oss << "error (" << pos << "): ";
    } else {
        oss << "error: ";
    }
    oss << msg;
    detail::user_debug_function(oss.str(), detail::debug_closure);
}

rectf detail::pdfrectangle_to_rectf(const PDFRectangle &pdfrect)
{
    return rectf(pdfrect.x1, pdfrect.y1, pdfrect.x2 - pdfrect.x1, pdfrect.y2 - pdfrect.y1);
}

ustring detail::unicode_GooString_to_ustring(const GooString *str)
{
    const char *data = str->getCString();
    const int len = str->getLength();

    int i = 0;
    bool is_unicode = false;

    printf("<");
    for (size_t j = 0; j < len; j++) {
        printf("%02x", (unsigned char)data[j]);
    }
    printf(">\n");

    if ((data[0] & 0xff) == 0xfe && (len > 1 && (data[1] & 0xff) == 0xff)) {
        is_unicode = true;
        i = 2;
    }
    ustring::size_type ret_len = len - i;
    if (is_unicode) {
        ret_len >>= 1;
    }
    ustring ret(ret_len, 0);
    size_t ret_index = 0;
    ustring::value_type u;

    if (is_unicode) {
        while (i < len) {
            u = ((data[i] & 0xff) << 8) | (data[i + 1] & 0xff);
            i += 2;
            ret[ret_index++] = u;
        }
    } else {
        while (i < len) {
            u = pdfDocEncoding[ data[i] & 0xff ];
            ++i;
            ret[ret_index++] = u;
        }
    }

    printf("<");
    for (size_t j = 0; j < ret_index; j ++) {
        printf("\\u%04x", (unsigned short)ret[j]);
    }
    printf(">\n");

    return ret;
}

ustring detail::unicode_to_ustring(const Unicode *u, int length)
{
    ustring str(length, 0);
    ustring::iterator it = str.begin();
    const Unicode *uu = u;
    for (int i = 0; i < length; ++i) {
        *it++ = ustring::value_type(*uu++ & 0xffff);
    }
    return str;
}

GooString* detail::ustring_to_unicode_GooString(const ustring &str)
{
    const size_t len = str.size() * 2 + 2;
    const ustring::value_type *me = str.data();
    byte_array ba(len);
    ba[0] = (char)0xfe;
    ba[1] = (char)0xff;
    for (size_t i = 0; i < str.size(); ++i, ++me) {
        ba[i * 2 + 2] = ((*me >> 8) & 0xff);
        ba[i * 2 + 3] = (*me & 0xff);
    }
    GooString *goo = new GooString(&ba[0], len);
    return goo;
}
