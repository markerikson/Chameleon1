


/*
    taTelnet - A cross-platform telnet program.
    Copyright (c) 2000 Derry Bryson.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact Information:

       Technology Associates, Inc.
       Attn:  Derry Bryson
       959 W. 5th Street
       Reno, NV  89503
       USA

       derry@techass.com
*/

#ifdef __GNUG__
    #pragma implementation "wxterm.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/timer.h>

#include <ctype.h>

#include "../network/gterm.hpp"
#include "../network/gtelnet.hpp"
#include "wxterm.h"

#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void WinMessageBeep();

#define CURSOR_BLINK_DEFAULT_TIMEOUT	300
#define CURSOR_BLINK_MAX_TIMEOUT	2000

/*
**  Keycode translation tables
*/
wxTerm::TermKeyMap wxTerm::keyMapTable[] =
{
  { WXK_BACK, GTelnet::KEY_BACK },
  { WXK_TAB, GTelnet::KEY_TAB },
  { WXK_RETURN, GTelnet::KEY_RETURN },
  { WXK_ESCAPE, GTelnet::KEY_ESCAPE },
  { WXK_SPACE, GTelnet::KEY_SPACE },
  { WXK_LEFT, GTelnet::KEY_LEFT },
  { WXK_UP, GTelnet::KEY_UP },
  { WXK_RIGHT, GTelnet::KEY_RIGHT },
  { WXK_DOWN, GTelnet::KEY_DOWN },
//  { WXK_DIVIDE, GTelnet::KEY_DIVIDE },
//  { WXK_MULTIPLY, GTelnet::KEY_MULTIPLY },
//  { WXK_SUBTRACT, GTelnet::KEY_SUBTRACT },
//  { WXK_ADD, GTelnet::KEY_ADD },
  { WXK_HOME, GTelnet::KEY_HOME },
  { WXK_END, GTelnet::KEY_END },
  { WXK_PAGEUP, GTelnet::KEY_PAGEUP },
  { WXK_PAGEDOWN, GTelnet::KEY_PAGEDOWN },
  { WXK_INSERT, GTelnet::KEY_INSERT },
  { WXK_DELETE, GTelnet::KEY_DELETE },
  { WXK_NEXT, GTelnet::KEY_NEXT },
  { WXK_PRIOR, GTelnet::KEY_PRIOR },
//  { WXK_NUMPAD0, GTelnet::KEY_NUMPAD0 },
//  { WXK_NUMPAD1, GTelnet::KEY_NUMPAD1 },
//  { WXK_NUMPAD2, GTelnet::KEY_NUMPAD2 },
//  { WXK_NUMPAD3, GTelnet::KEY_NUMPAD3 },
//  { WXK_NUMPAD4, GTelnet::KEY_NUMPAD4 },
//  { WXK_NUMPAD5, GTelnet::KEY_NUMPAD5 },
//  { WXK_NUMPAD6, GTelnet::KEY_NUMPAD6 },
//  { WXK_NUMPAD7, GTelnet::KEY_NUMPAD7 },
//  { WXK_NUMPAD8, GTelnet::KEY_NUMPAD8 },
//  { WXK_NUMPAD9, GTelnet::KEY_NUMPAD9 },
//  { WXK_DECIMAL, GTelnet::KEY_NUMPAD_DECIMAL },
  { WXK_F1, GTelnet::KEY_F1 },
  { WXK_F2, GTelnet::KEY_F2 },
  { WXK_F3, GTelnet::KEY_F3 },
  { WXK_F4, GTelnet::KEY_F4 },
  { WXK_F5, GTelnet::KEY_F5 },
  { WXK_F6, GTelnet::KEY_F6 },
  { WXK_F7, GTelnet::KEY_F7 },
  { WXK_F8, GTelnet::KEY_F8 },
  { WXK_F9, GTelnet::KEY_F9 },
  { WXK_F10, GTelnet::KEY_F10 },
  { WXK_F11, GTelnet::KEY_F11 },
  { WXK_F12, GTelnet::KEY_F12 },
  { (wxKeyCode)0, GTelnet::KEY_NULL }
};

static unsigned char
  xCharMap[] =
  {
    0, // 0
    1, // 1
    2, // 2
    3, // 3
    1, // 4
    5, // 5
    6, // 6
    7, // 7
    8, // 8
    9, // 9
    10, // 10
    11, // 11
    12, // 12
    13, // 13
    14, // 14
    15, // 15
    62, // 16
    60, // 17
    18, // 18
    19, // 19
    20, // 20
    21, // 21
    22, // 22
    23, // 23
    24, // 24
    25, // 25
    26, // 26
    27, // 27
    28, // 28
    29, // 29
    94, // 30
    31, // 31
    32, // 32
    33, // 33
    34, // 34
    35, // 35
    36, // 36
    37, // 37
    38, // 38
    39, // 39
    40, // 40
    41, // 41
    42, // 42
    43, // 43
    44, // 44
    45, // 45
    46, // 46
    47, // 47
    48, // 48
    49, // 49
    50, // 50
    51, // 51
    52, // 52
    53, // 53
    54, // 54
    55, // 55
    56, // 56
    57, // 57
    58, // 58
    59, // 59
    60, // 60
    61, // 61
    62, // 62
    63, // 63
    64, // 64
    65, // 65
    66, // 66
    67, // 67
    68, // 68
    69, // 69
    70, // 70
    71, // 71
    72, // 72
    73, // 73
    74, // 74
    75, // 75
    76, // 76
    77, // 77
    78, // 78
    79, // 79
    80, // 80
    81, // 81
    82, // 82
    83, // 83
    84, // 84
    85, // 85
    86, // 86
    87, // 87
    88, // 88
    89, // 89
    90, // 90
    91, // 91
    92, // 92
    93, // 93
    94, // 94
    95, // 95
    96, // 96
    97, // 97
    98, // 98
    99, // 99
    100, // 100
    101, // 101
    102, // 102
    103, // 103
    104, // 104
    105, // 105
    106, // 106
    107, // 107
    108, // 108
    109, // 109
    110, // 110
    111, // 111
    112, // 112
    113, // 113
    114, // 114
    115, // 115
    116, // 116
    117, // 117
    118, // 118
    119, // 119
    120, // 120
    121, // 121
    122, // 122
    123, // 123
    124, // 124
    125, // 125
    126, // 126
    127, // 127
    128, // 128
    129, // 129
    130, // 130
    131, // 131
    132, // 132
    133, // 133
    134, // 134
    135, // 135
    136, // 136
    137, // 137
    138, // 138
    139, // 139
    140, // 140
    141, // 141
    142, // 142
    143, // 143
    144, // 144
    145, // 145
    146, // 146
    147, // 147
    148, // 148
    149, // 149
    150, // 150
    151, // 151
    152, // 152
    153, // 153
    154, // 154
    155, // 155
    156, // 156
    157, // 157
    158, // 158
    159, // 159
    160, // 160
    161, // 161
    162, // 162
    163, // 163
    164, // 164
    165, // 165
    166, // 166
    167, // 167
    168, // 168
    169, // 169
    170, // 170
    171, // 171
    172, // 172
    173, // 173
    174, // 174
    175, // 175
    2, // 176
    2, // 177
    2, // 178
    25, // 179
    22, // 180
    22, // 181
    22, // 182
    12, // 183
    12, // 184
    22, // 185
    25, // 186
    12, // 187
    11, // 188
    11, // 189
    11, // 190
    12, // 191
    14, // 192
    23, // 193
    24, // 194
    21, // 195
    18, // 196
    15, // 197
    21, // 198
    21, // 199
    14, // 200
    13, // 201
    23, // 202
    24, // 203
    21, // 204
    18, // 205
    15, // 206
    23, // 207
    23, // 208
    24, // 209
    24, // 210
    14, // 211
    14, // 212
    13, // 213
    13, // 214
    15, // 215
    15, // 216
    11, // 217
    13, // 218
    0, // 219
    220, // 220
    221, // 221
    222, // 222
    223, // 223
    224, // 224
    225, // 225
    226, // 226
    227, // 227
    228, // 228
    229, // 229
    230, // 230
    231, // 231
    232, // 232
    233, // 233
    234, // 234
    235, // 235
    236, // 236
    237, // 237
    238, // 238
    239, // 239
    240, // 240
    241, // 241
    242, // 242
    243, // 243
    244, // 244
    245, // 245
    246, // 246
    247, // 247
    248, // 248
    249, // 249
    250, // 250
    251, // 251
    252, // 252
    253, // 253
    254, // 254
    255  // 255
  };

BEGIN_EVENT_TABLE(wxTerm, wxWindow)
  EVT_PAINT						(wxTerm::OnPaint)
  EVT_CHAR						(wxTerm::OnChar)
  EVT_LEFT_DOWN					(wxTerm::OnLeftDown)
  EVT_LEFT_UP					(wxTerm::OnLeftUp)
  EVT_MOTION					(wxTerm::OnMouseMove)
  EVT_TIMER						(-1, wxTerm::OnTimer)
#if 0
  EVT_KEY_DOWN(wxTerm::OnKeyDown)
#endif

  EVT_SIZE						(wxTerm::OnSize)
  EVT_SET_FOCUS					(wxTerm::OnGainFocus)
  EVT_KILL_FOCUS				(wxTerm::OnLoseFocus)
END_EVENT_TABLE()

wxTerm::wxTerm(wxWindow* parent, wxWindowID id,
               const wxPoint& pos,
               int width, int height,
               const wxString& name) :
  //wxScrolledWindow(parent, id, pos, wxSize(-1, -1), wxHSCROLL | wxVSCROLL | wxWANTS_CHARS, name),
  wxWindow(parent, id, pos, wxSize(-1, -1), wxWANTS_CHARS, name),
  GTerm/*lnet*/(width, height)
{
  int
    i;

  m_inUpdateSize = false;
  m_isActive = false;
  m_scrollBarWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_ARROW_X);

  m_init = 1;

  m_curDC = 0;

  m_charsInLine = width;
  m_linesDisplayed = height;

  m_selecting = FALSE;
  m_selx1 = m_sely1 = m_selx2 = m_sely2 = 0;
  m_marking = FALSE;
  m_curX = -1;
  m_curY = -1;
  m_curBlinkRate = CURSOR_BLINK_DEFAULT_TIMEOUT;
  m_timer.SetOwner(this);
  if(m_curBlinkRate)
    m_timer.Start(m_curBlinkRate);

  m_boldStyle = COLOR;

  GetDefVTColors(m_vt_colors);
  GetDefPCColors(m_pc_colors);

  m_colors = m_vt_colors;

  SetBackgroundColour(m_colors[0]);

  for(i = 0; i < 16; i++)
    m_vt_colorPens[i] = wxPen(m_vt_colors[i], 1, wxSOLID);

  for(i = 0; i < 16; i++)
    m_pc_colorPens[i] = wxPen(m_pc_colors[i], 1, wxSOLID);

  m_colorPens = m_vt_colorPens;

  m_width = width;
  m_height = height;

  m_printerFN = 0;
  m_printerName = 0;

 

  m_normalFont = GetFont();
  m_underlinedFont = GetFont();
  m_underlinedFont.SetUnderlined(TRUE);
  m_boldFont = GetFont();
  m_boldFont.SetWeight(wxBOLD);
  m_boldUnderlinedFont = m_boldFont;
  m_boldUnderlinedFont.SetUnderlined(TRUE);

  m_bitmap = 0;

  
  //ResizeTerminal(width, height);

  
  //SetVirtualSize(m_charWidth * 80, m_charHeight * 100);
  //SetScrollRate(m_charWidth, m_charHeight);

  m_init = 0;

  SetCursor(wxCursor(wxCURSOR_IBEAM));

  wxFont monospacedFont(10, wxMODERN, wxNORMAL, wxNORMAL, false, "Courier New");
  SetFont(monospacedFont);


  // 10pt Courier New is 8 pixels wide and 16 pixels high... set up
  // a default client size to match
  SetClientSize(m_charsInLine * 8, m_linesDisplayed * 16);
  UpdateSize();
}

wxTerm::~wxTerm()
{
  if(m_bitmap)
  {
    m_memDC.SelectObject(wxNullBitmap);
    delete m_bitmap;
  }
}

void
wxTerm::SetBoldStyle(wxTerm::BOLDSTYLE boldStyle)
{
  wxColour
    colors[16];

  if(boldStyle == DEFAULT)
    boldStyle = COLOR;

  m_boldStyle = boldStyle;
//  GetDefVTColors(colors, m_boldStyle);
//  SetVTColors(colors);
  Refresh();
}

bool
wxTerm::SetFont(const wxFont& font)
{
  m_init = 1;
  
  wxWindow::SetFont(font);
  m_normalFont = font;
  m_underlinedFont = font;
  m_underlinedFont.SetUnderlined(TRUE);
  m_boldFont = GetFont();
  m_boldFont.SetWeight(wxBOLD);
  m_boldUnderlinedFont = m_boldFont;
  m_boldUnderlinedFont.SetUnderlined(TRUE);
  m_init = 0;
  
  ResizeTerminal(m_width, m_height);
  Refresh();
  
  return TRUE;
}

void
wxTerm::GetDefVTColors(wxColour colors[16], wxTerm::BOLDSTYLE boldStyle)
{
  if(boldStyle == DEFAULT)
    boldStyle = m_boldStyle;

  if(boldStyle != COLOR)
  {
    colors[0] = wxColour(0, 0, 0);                             // black
    colors[1] = wxColour(255, 0, 0);                           // red
    colors[2] = wxColour(0, 255, 0);                           // green
    colors[3] = wxColour(255, 0, 255);                         // yellow
    colors[4] = wxColour(0, 0, 255);                           // blue
    colors[5] = wxColour(255, 255, 0);                         // magenta
    colors[6] = wxColour(0, 255, 255);                         // cyan
    colors[7] = wxColour(255, 255, 255);                       // white
    colors[8] = wxColour(0, 0, 0);                             // black
    colors[9] = wxColour(255, 0, 0);                           // red
    colors[10] = wxColour(0, 255, 0);                          // green
    colors[11] = wxColour(255, 0, 255);                        // yellow
    colors[12] = wxColour(0, 0, 255);                          // blue
    colors[13] = wxColour(255, 255, 0);                        // magenta
    colors[14] = wxColour(0, 255, 255);                        // cyan
    colors[15] = wxColour(255, 255, 255);                      // white
  }
  else
  {
    colors[0] = wxColour(0, 0, 0);                             // black
    colors[1] = wxColour(170, 0, 0);                           // red
    colors[2] = wxColour(0, 170, 0);                           // green
    colors[3] = wxColour(170, 0, 170);                         // yellow
    colors[4] = wxColour(0, 0, 170);                           // blue
    colors[5] = wxColour(170, 170, 0);                         // magenta
    colors[6] = wxColour(0, 170, 170);                         // cyan
    colors[7] = wxColour(192, 192, 192);                       // white
//    colors[7] = wxColour(170, 170, 170);                       // white
#if 0
    colors[8] = wxColour(85, 85, 85);                          // bold black
    colors[9] = wxColour(255, 85, 85);                         // bold red
    colors[10] = wxColour(85, 255, 85);                        // bold green
    colors[11] = wxColour(255, 85, 255);                       // bold yellow
    colors[12] = wxColour(85, 85, 255);                        // bold blue
    colors[13] = wxColour(255, 255, 85);                       // bold magenta
    colors[14] = wxColour(85, 255, 255);                       // bold cyan
    colors[15] = wxColour(255, 255, 255);                      // bold white
#else
    colors[8] = wxColour(85, 85, 85);                          // bold black
    colors[9] = wxColour(255, 0, 0);                         // bold red
    colors[10] = wxColour(0, 255, 0);                        // bold green
    colors[11] = wxColour(255, 0, 255);                       // bold yellow
    colors[12] = wxColour(0, 0, 255);                        // bold blue
    colors[13] = wxColour(255, 255, 0);                       // bold magenta
    colors[14] = wxColour(0, 255, 255);                       // bold cyan
    colors[15] = wxColour(255, 255, 255);                      // bold white
#endif    
  }
}

void
wxTerm::GetVTColors(wxColour colors[16])
{
  int
    i;

  for(i = 0; i < 16; i++)
    colors[i] = m_vt_colors[i];
}

void
wxTerm::SetVTColors(wxColour colors[16])
{
  int
    i;

  m_init = 1;
  for(i = 0; i < 16; i++)
    m_vt_colors[i] = colors[i];

  if(!(GetMode() & PC))
    SetBackgroundColour(m_vt_colors[0]);

  for(i = 0; i < 16; i++)
    m_vt_colorPens[i] = wxPen(m_vt_colors[i], 1, wxSOLID);
  m_init = 0;

  Refresh();
}

void
wxTerm::GetDefPCColors(wxColour colors[16])
{
#if 0
  /*
  **  These colors need tweaking.  I'm sure they are not correct.
  */
  colors[0] = wxColour(0, 0, 0);                             // black
  colors[1] = wxColour(0, 0, 128);                           // blue
  colors[2] = wxColour(0, 128, 0);                           // green
  colors[3] = wxColour(0, 128, 128);                         // cyan
  colors[4] = wxColour(128, 0, 0);                           // red
  colors[5] = wxColour(128, 0, 128);                         // magenta
  colors[6] = wxColour(128, 128, 0);                         // brown
  colors[7] = wxColour(128, 128, 128);                       // white
  colors[8] = wxColour(64, 64, 64);                          // gray
  colors[9] = wxColour(0, 0, 255);                           // lt blue
  colors[10] = wxColour(0, 255, 0);                          // lt green
  colors[11] = wxColour(0, 255, 255);                        // lt cyan
  colors[12] = wxColour(255, 0, 0);                          // lt red
  colors[13] = wxColour(255, 0, 255);                        // lt magenta
  colors[14] = wxColour(255, 255, 0);                        // yellow
  colors[15] = wxColour(255, 255, 255);                      // white
#else
  /*
  **  These are much better
  */
  colors[0] = wxColour(0, 0, 0);                             // black
  colors[1] = wxColour(0, 0, 170);                           // blue
  colors[2] = wxColour(0, 170, 0);                           // green
  colors[3] = wxColour(0, 170, 170);                         // cyan
  colors[4] = wxColour(170, 0, 0);                           // red
  colors[5] = wxColour(170, 0, 170);                         // magenta
  colors[6] = wxColour(170, 170, 0);                         // brown
  colors[7] = wxColour(170, 170, 170);                       // white
#if 0
  colors[8] = wxColour(85, 85, 85);                          // gray
  colors[9] = wxColour(85, 85, 255);                         // lt blue
  colors[10] = wxColour(85, 255, 85);                        // lt green
  colors[11] = wxColour(85, 255, 255);                       // lt cyan
  colors[12] = wxColour(255, 85, 85);                        // lt red
  colors[13] = wxColour(255, 85, 255);                       // lt magenta
  colors[14] = wxColour(255, 255, 85);                       // yellow
  colors[15] = wxColour(255, 255, 255);                      // white
#else
  colors[8] = wxColour(50, 50, 50);                          // gray
  colors[9] = wxColour(0, 0, 255);                         // lt blue
  colors[10] = wxColour(0, 255, 0);                        // lt green
  colors[11] = wxColour(0, 255, 255);                       // lt cyan
  colors[12] = wxColour(255, 0, 0);                        // lt red
  colors[13] = wxColour(255, 0, 255);                       // lt magenta
  colors[14] = wxColour(255, 255, 0);                       // yellow
  colors[15] = wxColour(255, 255, 255);                      // white
#endif  
#endif
}

void
wxTerm::GetPCColors(wxColour colors[16])
{
  int
    i;

  for(i = 0; i < 16; i++)
    colors[i] = m_pc_colors[i];
}

void
wxTerm::SetPCColors(wxColour colors[16])
{
  int
    i;

  m_init = 1;
  for(i = 0; i < 16; i++)
    m_pc_colors[i] = colors[i];

  if(GetMode() & PC)
    SetBackgroundColour(m_pc_colors[0]);

  for(i = 0; i < 16; i++)
    m_pc_colorPens[i] = wxPen(m_pc_colors[i], 1, wxSOLID);
  m_init = 0;

  Refresh();
}

void
wxTerm::SetCursorBlinkRate(int rate)
{
  if(rate < 0 || rate > CURSOR_BLINK_MAX_TIMEOUT)
    return;

  m_init = 1;
  if(rate != m_curBlinkRate)
  {
    m_curBlinkRate = rate;
    if(!m_curBlinkRate)
      m_timer.Stop();
    else
      m_timer.Start(m_curBlinkRate);
  }
  m_init = 0;
}

void
wxTerm::OnChar(wxKeyEvent& event)
{
  if(!(GetMode() & PC) && event.AltDown())
    event.Skip();
  else
  {
	  if(GTerm::IsScrolledUp())
	  {
		  GTerm::Scroll(MAXHEIGHT, false);
		  GTerm::Update();
		  Refresh();
	  }

    int
      rc,
      keyCode = 0,
      len;

    unsigned char
      buf[10];

    /*
    **  Map control characters
    */
    if(event.ControlDown())
    {
      if(event.KeyCode() >= 'a' && event.KeyCode() <= 'z')
        keyCode = event.KeyCode() - 'a' + 1;
      else if(event.KeyCode() >= '[' && event.KeyCode() <= '_')
        keyCode = event.KeyCode() - '[' + 0x1b;
      else if(event.KeyCode() == '6')
        keyCode = 0x1e;
      else if(event.KeyCode() == '-')
        keyCode = 0x1f;
    }
    
    if(!keyCode && !(keyCode = MapKeyCode((int)event.KeyCode())))
    {
      /*
      **  If the keycode wasn't mapped in the table and it is a special
      **  key, then we just ignore it.
      */
      if(event.KeyCode() >= WXK_START)
      {
        event.Skip();
        return;
      }
      /*
      **  Otherwise, it must just be an ascii character
      */
      keyCode = (int)event.KeyCode();
    }

    if(GetMode() & PC)
      rc = TranslateKeyCode(keyCode, &len, (char *)buf, event.ShiftDown(),
                        event.ControlDown(), event.AltDown());
    else
      rc = TranslateKeyCode(keyCode, &len, (char *)buf);

    if(rc)
    {
      if((GetMode() & NEWLINE) && !(GetMode() & PC) && (buf[len - 1] == 10))
      {
        buf[len - 1] = 13;
        buf[len] = 10;
        len++;
      }
      ProcessOutput(len, buf);
      if((GetMode() & LOCALECHO) && !(GetMode() & PC))
        ProcessInput(len, buf);
    }
    else if(!(GetMode() & PC))
    {
      if((GetMode() & NEWLINE) && !(GetMode() & PC) && (keyCode == 10))
      {
        len = 2;
        buf[0] = 13;
        buf[1] = keyCode;
      }
      else
      {
        len = 1;
        buf[0] = keyCode;
      }
      ProcessOutput(len, buf);
      if((GetMode() & LOCALECHO) && !(GetMode() & PC))
        ProcessInput(len, buf);
    }
    else
      event.Skip();
  }
}

void
wxTerm::OnKeyDown(wxKeyEvent& event)
{
  if(!(GetMode() & PC) && event.AltDown())
    event.Skip();
  else if(event.AltDown())
  {
//    wxLogMessage("OnKeyDown() got KeyCode = %d", event.KeyCode());
//    if(event.KeyCode() != 309)
//      OnChar(event);
  }
  else
    event.Skip();
}

void
wxTerm::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC
    dc(this);

  m_curDC = &dc;
  ExposeArea(0, 0, m_width, m_height);
  m_curDC = 0;
}

void
wxTerm::OnLeftDown(wxMouseEvent& event)
{
	/*
  ClearSelection();
  m_selx1 = m_selx2 = event.GetX() / m_charWidth;
  m_sely1 = m_sely2 = event.GetY() / m_charHeight;
  m_selecting = TRUE;
  CaptureMouse();
  */
}

void
wxTerm::OnLeftUp(wxMouseEvent& event)
{
	/*
  m_selecting = FALSE;
  if(GetCapture() == this)
  {
	ReleaseMouse();
  }
  */
  
}

void
wxTerm::OnMouseMove(wxMouseEvent& event)
{
  if(m_selecting)
  {
    m_selx2 = event.GetX() / m_charWidth;
    if(m_selx2 >= Width())
      m_selx2 = Width() - 1;
    m_sely2 = event.GetY() / m_charHeight;
    if(m_sely2 >= Height())
      m_sely2 = Height() - 1;

    MarkSelection();
  }
}

void
wxTerm::ClearSelection()
{
  int
    x,
    y;

  wxClientDC
    *dc = 0;

  m_selx1 = m_sely1 = m_selx2 = m_sely2 = 0;

  if(!m_curDC)
  {
    dc = new wxClientDC(this);
    m_curDC = dc;
  }

  for(y = 0; y < Height(); y++)
    for(x = 0; x < Width(); x++)
      Select(x, y, 0);

  if(dc)
  {
	  this->wxWindow::Update();
	  
    m_curDC = 0;
    delete dc;
  }
}

void
wxTerm::MarkSelection()
{
  int
    x,
    y;

  wxClientDC
    *dc = 0;

  m_marking = TRUE;

  if(!m_curDC)
  {
    dc = new wxClientDC(this);
    m_curDC = dc;
  }

  for(y = 0; y < Height(); y++)
    for(x = 0; x < Width(); x++)
      Select(x, y, 0);

  if(m_sely1 == m_sely2)
  {
    if(m_selx1 >= m_selx2)
      for(x = m_selx1; x <= m_selx2; x++)
        Select(x, m_sely1, 1);
    else
      for(x = m_selx2; x >= m_selx1; x--)
        Select(x, m_sely1, 1);
  }
  else if(m_sely1 < m_sely2)
  {
    for(x = m_selx1; x < Width(); x++)
      Select(x, m_sely1, 1);

    for(y = m_sely1 + 1; y < m_sely2; y++)
      for(x = 0; x < Width(); x++)
        Select(x, y, 1);

    for(x = 0; x <= m_selx2; x++)
      Select(x, m_sely2, 1);
  }
  else
  {
    for(x = 0; x <= m_selx1; x++)
      Select(x, m_sely1, 1);

    for(y = m_sely2 + 1; y < m_sely1; y++)
      for(x = 0; x < Width(); x++)
        Select(x, y, 1);

    for(x = m_selx2; x < Width(); x++)
      Select(x, m_sely2, 1);
  }

  this->wxWindow::Update();

  if(dc)
  {
    m_curDC = 0;
    delete dc;
  }

  m_marking = FALSE;
}

bool
wxTerm::HasSelection()
{
  return(m_selx1 != m_selx2 || m_sely1 != m_sely2);
}

wxString
wxTerm::GetSelection()
{
  int
    x1,
    y1,
    x2,
    y2;

  wxString
    sel;

  if(m_sely1 <= m_sely2)
  {
    x1 = m_selx1;
    y1 = m_sely1;
    x2 = m_selx2;
    y2 = m_sely2;
  }
  else
  {
    x1 = m_selx2;
    y1 = m_sely2;
    x2 = m_selx1;
    y2 = m_sely1;
  }

  while(x1 != x2 || y1 != y2)
  {
    if(GetChar(x1, y1))
      sel.Append(GetChar(x1, y1));

    x1++;
    if(x1 == Width())
    {
      sel.Append('\n');
      x1 = 0;
      y1++;
    }
  }
  if(GetChar(x1, y1))
    sel.Append(GetChar(x1, y1));

  return sel;
}

void
wxTerm::SelectAll()
{
  m_selx1 = 0;
  m_sely1 = 0;
  m_selx2 = Width() - 1;
  m_sely2 = Height() - 1;
  MarkSelection();
}

/*
**  GTelnet stuff
*/
void
wxTerm::DrawText(int fg_color, int bg_color, int flags,
                 int x, int y, int len, unsigned char *string)
{
  int
    t;

  if(flags & BOLD && m_boldStyle == COLOR)
    fg_color = (fg_color % 8) + 8;

  if(flags & SELECTED)
  {
    fg_color = 0;
    bg_color = 15;
  }

  if(flags & INVERSE)
  {
    t = fg_color;
    fg_color = bg_color;
    bg_color = t;
  }

  if(!m_curDC)
    return;

#if defined(__WXGTK__) || defined(__WXMOTIF__)
  int
    i;

  for(i = 0; string[i]; i++)
    string[i] = xCharMap[string[i]];
#endif

  wxString
    str(string, len);

  if(m_boldStyle != FONT)
  {
    if(flags & UNDERLINE)
      m_curDC->SetFont(m_underlinedFont);
    else
      m_curDC->SetFont(m_normalFont);
  }
  else
  {
    if(flags & BOLD)
    {
      if(flags & UNDERLINE)
        m_curDC->SetFont(m_boldUnderlinedFont);
      else
        m_curDC->SetFont(m_boldFont);
    }
    else
    {
      if(flags & UNDERLINE)
        m_curDC->SetFont(m_underlinedFont);
      else
        m_curDC->SetFont(m_normalFont);
    }
  }

  x = x * m_charWidth;
  y = y * m_charHeight;
  m_curDC->SetBackgroundMode(wxSOLID);
  m_curDC->SetTextBackground(m_colors[bg_color]);
  m_curDC->SetTextForeground(m_colors[fg_color]);
  m_curDC->DrawText(str, x, y);
  if(flags & BOLD && m_boldStyle == OVERSTRIKE)
    m_curDC->DrawText(str, x + 1, y);
}

void
wxTerm::DoDrawCursor(int fg_color, int bg_color, int flags,
                   int x, int y, unsigned char c)
{
	if(GTerm::IsScrolledUp())
	{
		return;
	}
  int
    t;

  if(flags & BOLD && m_boldStyle == COLOR)
    fg_color = (fg_color % 8) + 8;

  if(flags & INVERSE)
  {
    t = fg_color;
    fg_color = bg_color;
    bg_color = t;
  }

  if(!m_curDC)
    return;

#if defined(__WXGTK__) || defined(__WXMOTIF__)
  c = xCharMap[c];
#endif

  wxString
    str((char)c);

  if(m_boldStyle != FONT)
  {
    if(flags & UNDERLINE)
      m_curDC->SetFont(m_underlinedFont);
    else
      m_curDC->SetFont(m_normalFont);
  }
  else
  {
    if(flags & BOLD)
    {
      if(flags & UNDERLINE)
        m_curDC->SetFont(m_boldUnderlinedFont);
      else
        m_curDC->SetFont(m_boldFont);
    }
    else
    {
      if(flags & UNDERLINE)
        m_curDC->SetFont(m_underlinedFont);
      else
        m_curDC->SetFont(m_normalFont);
    }
  }

  x = x * m_charWidth;
  y = y * m_charHeight;
  m_curDC->SetBackgroundMode(wxSOLID);
  m_curDC->SetTextBackground(m_colors[fg_color]);
  m_curDC->SetTextForeground(m_colors[bg_color]);
  m_curDC->DrawText(str, x, y);
  if(flags & BOLD && m_boldStyle == OVERSTRIKE)
    m_curDC->DrawText(str, x + 1, y);
}

void
wxTerm::DrawCursor(int fg_color, int bg_color, int flags,
                   int x, int y, unsigned char c)
{
  m_curX = x;
  m_curY = y;
  m_curFG = fg_color;
  m_curBG = bg_color,
  m_curFlags = flags;
  m_curChar = c;



  if(m_timer.IsRunning())
    m_timer.Stop();
  DoDrawCursor(fg_color, bg_color, flags, x, y, c);
  if(m_curBlinkRate)
  {
    m_timer.Start(m_curBlinkRate);
    m_curState = 1;
  }
}

void
wxTerm::OnTimer(wxTimerEvent& WXUNUSED(event))
{
  wxClientDC
    *dc = 0;

  if(m_init)
    return;
    
  if(m_curX == -1 || m_curY == -1)
    return;
    
  if(GetMode() & CURSORINVISIBLE)
  {
	  //wxLogDebug("Skipping cursor");
    return;
  }
  //wxLogDebug("Drawing cursor");
  if(!m_curDC)
  {
    dc = new wxClientDC(this);
    m_curDC = dc;
  }

  if(m_curBlinkRate)
  {
    m_curState++;
    if(m_curState & 1 && m_curX != -1 && m_curY != -1)
      DoDrawCursor(m_curFG, m_curBG, m_curFlags, m_curX, m_curY, m_curChar);
    else
      DoDrawCursor(m_curBG, m_curFG, m_curFlags, m_curX, m_curY, m_curChar);
  }

  if(dc)
  {
    delete dc;
    m_curDC = 0;
  }
}

void
wxTerm::MoveChars(int sx, int sy, int dx, int dy, int w, int h)
{
  if(!m_marking)
    ClearSelection();

  sx = sx * m_charWidth;
  sy = sy * m_charHeight;
  dx = dx * m_charWidth;
  dy = dy * m_charHeight;
  w = w * m_charWidth;
  h = h * m_charHeight;

  m_memDC.Blit(0, 0, w, h, m_curDC, sx, sy);
  m_curDC->Blit(dx, dy, w, h, &m_memDC, 0, 0);
}

void
wxTerm::ClearChars(int bg_color, int x, int y, int w, int h)
{
//  if(!m_marking)
//    ClearSelection();

  x = x * m_charWidth;
  y = y * m_charHeight;
  w = w * m_charWidth;
  h = h * m_charHeight;

  bool deleteDC = false;
  if(!m_curDC)
  {
	  m_curDC = new wxClientDC(this);
	  deleteDC = true;
  }
  m_curDC->SetPen(m_colorPens[bg_color]);
  m_curDC->SetBrush(wxBrush(m_colors[bg_color], wxSOLID));
  m_curDC->DrawRectangle(x, y, w /* + 1*/, h /*+ 1*/);

  if(deleteDC)
  {
	  delete m_curDC;
	  m_curDC = 0;
  }
}

void
wxTerm::ModeChange(int state)
{
  ClearSelection();

  if(state & GTelnet::PC)
  {
    m_colors = m_pc_colors;
    m_colorPens = m_pc_colorPens;
  }
  else
  {
    m_colors = m_vt_colors;
    m_colorPens = m_vt_colorPens;
  }
  GTerm/*lnet*/::ModeChange(state);
}

void
wxTerm::Bell()
{
#ifdef __WIN32__
  WinMessageBeep();
#else
  wxBell();
#endif
}

//void wxTerm::UpdateSize(int &termheight, int &linesReceived)
//void wxTerm::UpdateSize(wxSizeEvent &event)
void wxTerm::UpdateSize()
{
	//event.Skip();
	if(m_inUpdateSize)
	{		
		return;
	}

	m_inUpdateSize = true;
	int charWidth, charHeight;

	wxClientDC* dc = new wxClientDC(this);

	if(!m_curDC)
	{			
		m_curDC = dc;
	}
	
	dc->SetFont(m_normalFont);
	dc->GetTextExtent("M", &charWidth, &charHeight);
	wxSize currentClientSize = GetClientSize();
	int numCharsInLine = currentClientSize.GetX() / charWidth;
	int numLinesShown = currentClientSize.GetY() / charHeight;

	

	if( (numCharsInLine != m_charsInLine) || (numLinesShown != m_linesDisplayed))
	{
		wxString message;

		//message.Printf("numCharsInLine: %d, numLinesShown: %d", numCharsInLine, numLinesShown);
		//wxLogDebug(message);


		// FINALLY!  Finally killed the memory leak!  The problem is that somehow a size event
		// was generating negative numbers for these values, which led to weird things happening.
		if( (numCharsInLine > 0) && (numLinesShown > 0))
		{
			m_charsInLine = numCharsInLine;
			m_linesDisplayed = numLinesShown;
			ResizeTerminal(numCharsInLine, numLinesShown);	
		}

		// TODO Obviously this isn't working right.  I'll come back to it when I tackle
		//		the line history issue
		//GTerm::ExposeArea(0, 0, GTerm::Width() - 1, GTerm::Height() - 1);
		//Refresh();
		//GTerm::Update();	
	}

	m_inUpdateSize = false;

	if(dc)
	{
		delete dc;
		m_curDC = 0;
	}
}

void
wxTerm::ResizeTerminal(int width, int height)
{
  int
    w,
    h;

  ClearSelection();

  /*
  **  Determine window size from current font
  */
  wxClientDC
    dc(this);

  if(m_boldStyle != FONT)
    dc.SetFont(m_normalFont);
  else
    dc.SetFont(m_boldFont);
  dc.GetTextExtent("M", &m_charWidth, &m_charHeight);
  w = width * m_charWidth;
  h = height * m_charHeight;

  /*
  **  Create our bitmap for copying
  */
  if(m_bitmap)
  {
    m_memDC.SelectObject(wxNullBitmap);
    delete m_bitmap;
  }
  m_bitmap = new wxBitmap(w, h);
  m_memDC.SelectObject(*m_bitmap);

  /*
  **  Set window size
  */
#if defined(__WXGTK__) || defined(__WXMOTIF__)
  SetSize(w, h + 4);
#else
  SetSize(w, h);
#endif

  /*
  **  Set terminal size
  */
  GTerm/*lnet*/::ResizeTerminal(width, height);
  m_width = width;
  m_height = height;

  /*
  **  Send event
  */
  if(!m_init)
  {
    wxCommandEvent e(wxEVT_COMMAND_TERM_RESIZE, GetId());
    e.SetEventObject(this);
    GetParent()->GetEventHandler()->ProcessEvent(e);
  }
}

void
wxTerm::RequestSizeChange(int w, int h)
{
  ResizeTerminal(w, h);
}

void
wxTerm::ProcessInput(int len, unsigned char *data)
{
  wxClientDC
    dc(this);

  ClearSelection();
  m_curDC = &dc;
  GTerm/*lnet*/::ProcessInput(len, data);
  m_curDC = 0;
}

int
wxTerm::MapKeyCode(int keyCode)
{
  int
    i;

  for(i = 0; keyMapTable[i].keyCode; i++)
    if(keyMapTable[i].keyCode == keyCode)
      return keyMapTable[i].VTKeyCode;
  return 0;
}

void
wxTerm::SelectPrinter(char *PrinterName)
{
  if(m_printerFN)
  {
    if(m_printerName[0] == '#')
      fclose(m_printerFN);
    else
#if defined(__WXGTK__) || defined(__WXMOTIF__)
      pclose(m_printerFN);
#endif
#if defined(__WXMSW__)
      fclose(m_printerFN);
#endif

    m_printerFN = 0;
  }

  if(m_printerName)
  {
    free(m_printerName);
    m_printerName = 0;
  }

  if(strlen(PrinterName))
  {
    m_printerName = strdup(PrinterName);
  }
}

void
wxTerm::PrintChars(int len, unsigned char *data)
{
  char
    pname[100];

  if(!m_printerFN)
  {
    if(!m_printerName)
      return;

    if(m_printerName[0] == '#')
    {
#if defined(__WXGTK__) || defined(__WXMOTIF__)
      sprintf(pname, "/dev/lp%d", m_printerName[1] - '0');
#endif
#if defined(__WXMSW__)
      sprintf(pname, "lpt%d", m_printerName[1] - '0' + 1);
#endif
      m_printerFN = fopen(pname, "wb");
    }
    else
    {
#if defined(__WXGTK__) || defined(__WXMOTIF__)
      sprintf(pname, "lpr -P%s", m_printerName);
      m_printerFN = popen(pname, "w");
#endif
#if defined(__WXMSW__)
      m_printerFN = fopen(m_printerName, "wb");
#endif
    }
  }
  
  if(m_printerFN)
  {
    fwrite(data, len, 1, m_printerFN);
  }
}

void wxTerm::OnActivate(wxActivateEvent &event)
{
	m_isActive = event.GetActive();
}

void wxTerm::OnGainFocus(wxFocusEvent &event)
{
	this->clear_mode_flag(CURSORINVISIBLE);
	//wxLogDebug("Gained focus");
	GTerm::Update();
}

void wxTerm::OnLoseFocus(wxFocusEvent &event)
{
	this->set_mode_flag(CURSORINVISIBLE);
	//wxLogDebug("Lost focus");
	GTerm::Update();
}

void wxTerm::ScrollTerminal(int numLines, bool scrollUp /* = true */)
{
	GTerm::Scroll(numLines, scrollUp);
	Refresh();

}

void wxTerm::OnSize(wxSizeEvent &event)
{
	UpdateSize();
}