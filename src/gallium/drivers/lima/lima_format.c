/*
 * Copyright (c) 2011-2013 Luc Verhaegen <libv@skynet.be>
 * Copyright (c) 2018-2019 Lima Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdint.h>
#include <stdlib.h>

#include <util/macros.h>

#include "lima_format.h"

#define LIMA_TEXEL_FORMAT_L8           0x09
#define LIMA_TEXEL_FORMAT_A8           0x0a
#define LIMA_TEXEL_FORMAT_I8           0x0b
#define LIMA_TEXEL_FORMAT_BGR_565      0x0e
#define LIMA_TEXEL_FORMAT_BGRA_5551    0x0f
#define LIMA_TEXEL_FORMAT_BGRA_4444    0x10
#define LIMA_TEXEL_FORMAT_L8A8         0x11
#define LIMA_TEXEL_FORMAT_L16          0x12
#define LIMA_TEXEL_FORMAT_A16          0x13
#define LIMA_TEXEL_FORMAT_I16          0x14
#define LIMA_TEXEL_FORMAT_RGB_888      0x15
#define LIMA_TEXEL_FORMAT_RGBA_8888    0x16
#define LIMA_TEXEL_FORMAT_RGBX_8888    0x17
#define LIMA_TEXEL_FORMAT_ETC1_RGB8    0x20
#define LIMA_TEXEL_FORMAT_Z24X8        0x2c
/* This format is only used for depth/stencil reload */
#define LIMA_TEXEL_FORMAT_Z24S8_RLD    0x32
#define LIMA_TEXEL_FORMAT_NONE         -1

#define LIMA_PIXEL_FORMAT_B5G6R5       0x00
#define LIMA_PIXEL_FORMAT_B5G5R5A1     0x01
#define LIMA_PIXEL_FORMAT_B4G4R4A4     0x02
#define LIMA_PIXEL_FORMAT_B8G8R8A8     0x03
#define LIMA_PIXEL_FORMAT_Z16          0x0e
#define LIMA_PIXEL_FORMAT_Z24S8        0x0f
#define LIMA_PIXEL_FORMAT_NONE         -1

struct lima_format {
   bool present;
   int texel;
   int pixel;
   bool swap_r_b;
   uint32_t channel_layout;
};

#define LIMA_FORMAT(pipe, tex, pix, swap, ch_layout)      \
   [PIPE_FORMAT_##pipe] = {                               \
      .present = true, .texel = LIMA_TEXEL_FORMAT_##tex,  \
      .pixel = LIMA_PIXEL_FORMAT_##pix, .swap_r_b = swap, \
      .channel_layout = ch_layout,                        \
   }

static const struct lima_format lima_format_table[] = {
   LIMA_FORMAT(R8G8B8A8_UNORM,     RGBA_8888, B8G8R8A8, true,  0x8888),
   LIMA_FORMAT(B8G8R8A8_UNORM,     RGBA_8888, B8G8R8A8, false, 0x8888),
   LIMA_FORMAT(R8G8B8A8_SRGB,      RGBA_8888, B8G8R8A8, true,  0x8888),
   LIMA_FORMAT(B8G8R8A8_SRGB,      RGBA_8888, B8G8R8A8, false, 0x8888),
   LIMA_FORMAT(R8G8B8X8_UNORM,     RGBX_8888, B8G8R8A8, true,  0x8888),
   LIMA_FORMAT(B8G8R8X8_UNORM,     RGBX_8888, B8G8R8A8, false, 0x8888),
   LIMA_FORMAT(B5G6R5_UNORM,       BGR_565,   B5G6R5,   false, 0x8565),
   /* BGRA_5551 seems to need channel layout 0x8565, it's not a typo */
   LIMA_FORMAT(B5G5R5A1_UNORM,     BGRA_5551, B5G5R5A1, false, 0x8565),
   LIMA_FORMAT(B4G4R4A4_UNORM,     BGRA_4444, B4G4R4A4, false, 0x8444),
   LIMA_FORMAT(Z24_UNORM_S8_UINT,  Z24X8,     Z24S8,    false, 0x0000),
   LIMA_FORMAT(Z24X8_UNORM,        Z24X8,     Z24S8,    false, 0x0000),
   LIMA_FORMAT(L16_UNORM,          L16,       NONE,     false, 0x0000),
   LIMA_FORMAT(L8_UNORM,           L8,        NONE,     false, 0x0000),
   LIMA_FORMAT(A16_UNORM,          A16,       NONE,     false, 0x0000),
   LIMA_FORMAT(A8_UNORM,           A8,        NONE,     false, 0x0000),
   LIMA_FORMAT(I16_UNORM,          I16,       NONE,     false, 0x0000),
   LIMA_FORMAT(I8_UNORM,           I8,        NONE,     false, 0x0000),
   LIMA_FORMAT(L8A8_UNORM,         L8A8,      NONE,     false, 0x0000),
   LIMA_FORMAT(ETC1_RGB8,          ETC1_RGB8, NONE,     false, 0x8888),
};

static const struct lima_format *
get_format(enum pipe_format f)
{
   if (f >= ARRAY_SIZE(lima_format_table) ||
       !lima_format_table[f].present)
      return NULL;

   return lima_format_table + f;
}

bool
lima_format_texel_supported(enum pipe_format f)
{
   const struct lima_format *lf = get_format(f);

   if (!lf)
      return false;

   return lf->texel != LIMA_TEXEL_FORMAT_NONE;
}

bool
lima_format_pixel_supported(enum pipe_format f)
{
   const struct lima_format *lf = get_format(f);

   if (!lf)
      return false;

   return lf->pixel != LIMA_PIXEL_FORMAT_NONE;
}

int
lima_format_get_texel(enum pipe_format f)
{
   return lima_format_table[f].texel;
}

int
lima_format_get_texel_reload(enum pipe_format f)
{
   switch (f) {
   case PIPE_FORMAT_Z24_UNORM_S8_UINT:
   case PIPE_FORMAT_Z24X8_UNORM:
      return LIMA_TEXEL_FORMAT_Z24S8_RLD;
   default:
      return lima_format_get_texel(f);
   }
}

int
lima_format_get_pixel(enum pipe_format f)
{
   return lima_format_table[f].pixel;
}

bool
lima_format_get_swap_rb(enum pipe_format f)
{
   return lima_format_table[f].swap_r_b;
}

uint32_t
lima_format_get_channel_layout(enum pipe_format f)
{
   return lima_format_table[f].channel_layout;
}