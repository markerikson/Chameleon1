// Copyright Timothy Miller, 1999

#include "gterm.hpp"
#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include <stdlib.h>

int GTerm::calc_color( int fg, int bg, int flags )
                           { return (flags & 15) | (fg << 4) | (bg << 8); }

void GTerm::update_changes ()
{
	//wxLogDebug("update_changes: NLC: %d", m_nextLineCounter);
    int yp, start_x, mx;

    int blank, c, x, y;

    // prevent recursion for scrolls which cause exposures
    if( doing_update )
        return;

    doing_update = 1;

    // first perform scroll-copy
    mx = scroll_bot - scroll_top + 1;

    if( !(mode_flags & TEXTONLY)
        && pending_scroll && pending_scroll < mx && -pending_scroll < mx )
    {
        if(pending_scroll < 0) 
		{ 
			MoveChars(0, scroll_top, 0, scroll_top - pending_scroll, width, 
				scroll_bot - scroll_top + pending_scroll + 1); 
		}
        else
        {
            MoveChars(0, scroll_top + pending_scroll, 0, scroll_top, width, 
				scroll_bot - scroll_top - pending_scroll + 1);
        }
    }

    pending_scroll = 0;

    // then update characters
    for( y = 0; y < height; y++ )
    {
        if( dirty_startx[y] >= MAXWIDTH )
            continue;

        yp = linenumbers[y] * MAXWIDTH;

        blank = !(mode_flags & TEXTONLY);

        start_x = dirty_startx[y];

        //c = color[yp + start_x];

		c = tm.GetColorAdjusted(y, start_x);

        for( x = start_x; x <= dirty_endx[y]; x++ )
        {
            if( text[yp + x] != 32 && text[yp + x] )
			//if( (tm[y][x] != 32) && (tm[y][x]))
                blank = 0;

            //if( c != color[yp + x] )
			if(c != tm.GetColorAdjusted(y, x))
            {
                if( !blank )
                {
					string substring = tm[y].substr(start_x, x - start_x);
					const char* substringText = substring.c_str();

                    if( mode_flags & PC )
					{						
						// note the DELIBERATE C-style cast... since I know that 
						// wxTerm won't mess with the text, this is okay
						DrawText( (c >> 4) & 0xf, (c >> 8) & 0xf, c, start_x, y, 
									x - start_x, (unsigned char*)substringText);

                        //DrawText((c >> 4) & 0xf, (c >> 8) & 0xf, c/*&15*/, start_x, y, x
                        //    - start_x, text + yp + start_x);
					}
                    else
					{
						// Ditto with the previous cast
						DrawText( (c >> 4) & 7, (c >> 8) & 7, c, start_x, y, 
									x - start_x, (unsigned char*)substringText);

                        //DrawText((c >> 4) & 7, (c >> 8) & 7, c/*&15*/, start_x, y, x
                        //    - start_x, text + yp + start_x);
					}
                }
                else
                {
                    ClearChars((c >> 8) & 7, start_x, y, x - start_x, 1);
                }

                start_x = x;

                //c = color[yp + x];
				c = tm.GetColorAdjusted(y, x);

                blank = !(mode_flags & TEXTONLY);

                //if( text[yp + x] != 32 && text[yp + x] )
				char ch = tm.GetCharAdjusted(y, x);
				if( (ch != 32) && (ch) )
                    blank = 0;
            }
        }

        if( !blank )
        {
			string line = tm.GetLineAdjusted(y);

			string substring = line.substr(start_x, x - start_x);//tm[y].substr(start_x, x - start_x);
			const char* substringText = substring.c_str();

            if( mode_flags & PC )
			{
				DrawText((c >> 4) & 0xf, (c >> 8) & 0xf, c/*&15*/, start_x, y, x
					- start_x, (unsigned char*)substringText);
				
                //DrawText((c >> 4) & 0xf, (c >> 8) & 0xf, c/*&15*/, start_x, y, x
                //    - start_x, text + yp + start_x);
			}
            else
			{
				DrawText((c >> 4) & 7, (c >> 8) & 7, c/*&15*/, start_x, y, x
					- start_x, (unsigned char*)substringText);

                //DrawText((c >> 4) & 7, (c >> 8) & 7, c/*&15*/, start_x, y, x
                //    - start_x, text + yp + start_x);
			}
        }
        else
        {
            ClearChars((c >> 8) & 7, start_x, y, x - start_x, 1);
        }

        dirty_endx[y] = 0;

        dirty_startx[y] = MAXWIDTH;
    }

    if( !(mode_flags & CURSORINVISIBLE) )
    {
        x = cursor_x;

        if( x >= width )
            x = width - 1;

        yp = linenumbers[cursor_y] * MAXWIDTH + x;

        //c = color[yp];
		c = tm.GetColorAdjusted(cursor_y, x);

		unsigned char cursorChar = tm[cursor_y][x];
        if( mode_flags & PC )
		{			
			DrawCursor((c >> 4) & 0xf, (c >> 8) & 0xf, c & 15, x, cursor_y, cursorChar);
            //DrawCursor((c >> 4) & 0xf, (c >> 8) & 0xf, c & 15, x, cursor_y, text[yp]);
		}
        else
		{
			//DrawCursor((c >> 4) & 7, (c >> 8) & 7, c & 15, x, cursor_y, cursorChar);
            DrawCursor((c >> 4) & 7, (c >> 8) & 7, c & 15, x, cursor_y, text[yp]);
		}
    }

    doing_update = 0;
}
// num is the number of lines to scroll at a time
void GTerm::scroll_region( int start_y, int end_y, int num )
{
	//wxLogDebug("scroll_region");
    int y, takey, fast_scroll, mx, clr, x, yp, c;

    short temp[MAXHEIGHT];

    unsigned char temp_sx[MAXHEIGHT], temp_ex[MAXHEIGHT];

    if( !num )
        return;

	// number of lines to scroll?
    mx = end_y - start_y + 1;

	// some sort of limiting factor, I think
    if( num > mx )
        num = mx;

    if( -num > mx )
        num = -mx;

	// all lines need to be scrolled, so zap it all?
    fast_scroll = (start_y
        == scroll_top && end_y == scroll_bot && !(mode_flags & TEXTONLY));
	// Does this deal with the "do multiple changes, then update" concept?
    if( fast_scroll )
        pending_scroll += num;

	// duplicate linenumbers
    memcpy(temp, linenumbers, sizeof(linenumbers));

	// if we're scrolling the entire then, then duplicate dirty_startx and dirty_endx
    if( fast_scroll )
    {
        memcpy(temp_sx, dirty_startx, sizeof(dirty_startx));

        memcpy(temp_ex, dirty_endx, sizeof(dirty_endx));
    }

    c = calc_color(fg_color, bg_color, mode_flags);

    // move the lines by renumbering where they point to
    if( num < mx && -num < mx )
	{
		wxString lnvalues = "s_r: ln = ";

		
		
		
        for( y = start_y; y <= end_y; y++ )
        {
            takey = y + num;

            clr = (takey < start_y) || (takey > end_y);

            if( takey < start_y )
                takey = end_y + 1 - (start_y - takey);

            if( takey > end_y )
                takey = start_y - 1 + (takey - end_y);

            linenumbers[y] = temp[takey];

			lnvalues << linenumbers[y] << " ";

			// 
            if( !fast_scroll || clr )
            {
                dirty_startx[y] = 0;

                dirty_endx[y] = width - 1;
            }
            else
            {
                dirty_startx[y] = temp_sx[takey];

                dirty_endx[y] = temp_ex[takey];
            }

            if( clr )
            {
                yp = linenumbers[y] * MAXWIDTH;
				int altY = m_nextLineCounter * MAXWIDTH;

                memset(text + yp, 32, width);

				//memset(alttext + altY, 32, width);
				//stringtext.replace(altY, width, width, ' ');

				/* IMPORTANT Hey... should scroll_region actually do anything
				 *			 if we're wrapping around?
				 */
				//textq[m_nextLineCounter].replace(width, width, ' ');

                for( x = 0; x < width; x++ )
                {
                    color[yp++] = c;
                }
            }
        }
		//wxLogDebug("scroll_region: start_y = %d, end_y = %d", start_y, end_y, lnvalues.c_str());
		//wxLogDebug(lnvalues);
	}
}

void GTerm::shift_text( int y, int start_x, int end_x, int num )
{
	wxLogDebug("shift_text");
    int x, yp, mx, c;

    if( !num )
        return;

    yp = linenumbers[y] * MAXWIDTH;
	int altY = m_nextLineCounter * MAXWIDTH;

	//int adjustedY = m_next

    mx = end_x - start_x + 1;

    if( num > mx )
        num = mx;

    if( -num > mx )
        num = -mx;

    if( num < mx && -num < mx )
    {
        if( num < 0 )
        {
            memmove(text + yp + start_x, text + yp + start_x - num, mx + num);

			string substring = tm[y].substr(start_x - num, mx + num);
			tm[y].replace(start_x, substring.size(), substring);
			//memmove(alttext + altY + start_x, alttext + altY + start_x - num, mx + num);
            
			//string substring = stringtext.substr(altY + start_x - num, mx + num);
			//stringtext.replace(altY + start_x, substring.size(), substring);

			//string substring2 = textq[

			int numToMove = (mx + num) << 1;
            memmove(color + yp + start_x, color
                //+ yp + start_x - num, (mx + num) << 1);
				+ yp + start_x - num, numToMove);

			for(int i = end_x; i >= start_x; i--)
			{
				//unsigned short tempColor = tm.GetColorAdjusted(y, i);
				//tm.SetColorAdjusted()
			}
        }
        else
        {
            memmove(text + yp + start_x + num, text + yp + start_x, mx - num);

			string substring = tm[y].substr(start_x, mx - num);
			tm[y].replace(start_x + num, substring.size(), substring);
			//memmove(alttext + altY + start_x + num, alttext + altY + start_x, mx - num);

			//string substring = stringtext.substr(altY + start_x, mx - num);
			//stringtext.replace(altY + start_x + num, substring.size(), substring);

			int numToMove = (mx - num) << 1;
            memmove(color + yp + start_x + num, color
                //+ yp + start_x, (mx - num) << 1);
				+ yp + start_x, numToMove);
        }
    }

    if(num < 0) { x = yp + end_x + num + 1; }
    else
    {
        x = yp + start_x;
    }

    num = abs(num);

    memset(text + x, 32, num);

	//memset(alttext + x, 32, num);
	//stringtext.replace(x, num, num, ' ');

    c = calc_color(fg_color, bg_color, mode_flags);

    while( num-- )
	{
        color[x++] = c;
		tm.SetColorAdjusted(y, x, c);
	}
    changed_line(y, start_x, end_x);
}

void GTerm::clear_area( int start_x, int start_y, int end_x, int end_y )
{
	//wxLogDebug("clear_area");
    int x, y, c, yp, w;

    c = calc_color(fg_color, bg_color, mode_flags);

    w = end_x - start_x + 1;

    if( w < 1 )
        return;

	

	if(start_y == end_y)
	{
		
		int num = end_x - start_x;
		
		wxLogDebug("clear_area: y: %d, start_x: %d, num: %d", start_y, start_x, num);

		if(num > 0)
		{
			tm[start_y].replace(start_x, num, num, ' ');
		}
		else
		{
			wxLogDebug("BAD THINGS!  num: %d, start_x: %d, end_x: %d, start_y: %d, end_y: %d",
						num, start_x, end_x, start_y, end_y);
		}
		
	}
	else
	{
		wxLogDebug("clear_area: wiping.  start_y: %d, end_y: %d", start_y, end_y);

		for(int i = start_y; i <= end_y; i++)
		{
			tm.AddNewLine();
		}
		/*
		string blankline;
		blankline.resize(width, ' ');
		
		for(int i = start_y; i <= end_y; i++)
		{
			tm.SetLine(i, blankline);
		}
		*/

	}

    for( y = start_y; y <= end_y; y++ )
    {
		if(linenumbers[y] < 0)
		{
			yp = (MAXHEIGHT - 1) * MAXWIDTH;
		}
		else
		{
			
			yp = linenumbers[y] * MAXWIDTH;
		}

		int altY = m_nextLineCounter * MAXWIDTH;

        memset(text + yp + start_x, 32, w);

		// TODO Do I need this here?  I'm guessing so in some fashion...
		//tm[y].replace(start_x, w, w, ' ');


		//memset(alttext + altY + start_x, 32, w);
		//stringtext.replace(altY + start_x, w, w, ' ');

        for( x = start_x; x <= end_x; x++ )
        {
            color[yp+x] = c;
        }

        changed_line(y, start_x, end_x);
    }
}

void GTerm::changed_line( int y, int start_x, int end_x )
{
    if( dirty_startx[y] > start_x )
        dirty_startx[y] = start_x;

    if( dirty_endx[y] < end_x )
        dirty_endx[y] = end_x;
}

void GTerm::move_cursor( int x, int y )
{
    if( cursor_x >= width )
        cursor_x = width - 1;

    changed_line(cursor_y, cursor_x, cursor_x);

    cursor_x = x;

    cursor_y = y;

	tm.SetCursorLine(y);
}

void GTerm::set_mode_flag( int flag )
{
    mode_flags |= flag;

    ModeChange(mode_flags);
}

void GTerm::clear_mode_flag( int flag )
{
    mode_flags &= ~flag;

    ModeChange(mode_flags);
}