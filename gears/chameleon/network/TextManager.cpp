#include "TextManager.h"

#include <iostream>
#include <stdlib.h>
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include "gterm.hpp"
#include "../common/debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

TextManager::TextManager(GTerm* parent, int width, int height, int maxWidth, int maxHeight /* = 50 */)
	:m_parent(parent), m_viewportWidth(width), m_viewportHeight(height), m_maxWidth(maxWidth), m_maxHeight(maxHeight)
{
	if(m_parent != NULL)
	{
		Reset();
	}
	
}

void TextManager::Reset()
{
	m_blankline.resize(m_maxWidth, ' ');
	m_text = deque<string>(m_maxHeight, m_blankline);

	// black background, white text
	m_blankColor = 112;
	vector<unsigned short> colorline(m_maxWidth, m_blankColor);
	m_color = deque< vector< unsigned short> >(m_maxHeight, colorline);

	m_bottomLine = m_maxHeight - 1;	
	m_topLine = m_bottomLine - m_viewportHeight + 1;
	m_numLinesScrolledUp = 0;

	m_cursorLine = 0;
	m_linesReceived = 1;
}

void TextManager::AddNewLine()
{
	AddNewLine(m_blankline);
}

void TextManager::AddNewLine(string newline)
{
	if(newline.size() != m_maxWidth)
	{
		newline.resize(m_maxWidth, ' ');
	}
	m_text.push_back(newline);

	vector<unsigned short> linecolors;
	linecolors.resize(m_maxWidth, m_blankColor);
	m_color.push_back(linecolors);

	m_linesReceived++;

	if((int)m_text.size() > m_maxHeight)
	{
		cout << "Popping front" << endl;
		m_text.pop_front();
		m_color.pop_front();
	}
	else
	{
		if(m_bottomLine < (m_maxHeight - 1))
		{
			m_bottomLine++;
			m_topLine = m_bottomLine - m_viewportHeight + 1;
		}
		//printf("Bottom: %d, top: %d\n", m_bottomLine, m_topLine);
	}
}

string &TextManager::operator [](int index)
{
	if(index != m_cursorLine)
	{
		//wxLogDebug("m_cursorLine: %d, index: %d", m_cursorLine, index);
	}
	return GetLineAdjusted(index);
}

void TextManager::Scroll(int numLines, bool scrollUp)
{
	int actualLinesToScroll = numLines;
	
	if(scrollUp)
	{
		if( ( (m_topLine - m_numLinesScrolledUp) == 0) || 
			( (m_numLinesScrolledUp + m_viewportHeight) >= (m_linesReceived -1)))
		{
			return;
		}
		if(m_topLine < actualLinesToScroll)
		{
			actualLinesToScroll = m_topLine;
		}

		int limiter = m_topLine - m_numLinesScrolledUp;
		//wxLogDebug("nLSU: %d, tL: %d, limiter: %d", m_numLinesScrolledUp, m_topLine, limiter);
		if(actualLinesToScroll > limiter)
		{
			actualLinesToScroll = limiter;
		}
		
		//m_topLine -= actualLinesToScroll;
		//m_bottomLine -= actualLinesToScroll;
		m_numLinesScrolledUp += actualLinesToScroll;
	}
	else
	{
		if( (m_bottomLine - m_numLinesScrolledUp) == (m_text.size() - 1))
		{
			return;
		}
		
		int linesBelow = m_text.size() - (m_bottomLine - m_numLinesScrolledUp) - 1;
		if( linesBelow < actualLinesToScroll)
		{
			actualLinesToScroll = linesBelow;
		}
		
		//m_topLine += actualLinesToScroll;
		//m_bottomLine += actualLinesToScroll;
		m_numLinesScrolledUp -= actualLinesToScroll;

		if(m_numLinesScrolledUp < 0)
		{
			m_numLinesScrolledUp = 0;
		}
	}	
}

int TextManager::GetSize()
{
	return m_text.size();
}

void TextManager::PrintViewport()
{
	wxLogDebug("Viewport (top = %d, bottom = %d)", m_topLine, m_bottomLine);

	for(int i = m_topLine; i <= m_bottomLine; i++)
	{
		wxLogDebug("%s", m_text[i].c_str());
	}
}

void TextManager::PrintContents()
{
	wxLogDebug("TextManager contents:");

	for(int i = 0; i < (int)m_text.size(); i++)
	{
		wxLogDebug("%s", m_text[i].c_str());
	}
}

void TextManager::Resize(int width, int height)
{
	if(height == m_viewportHeight)
	{
		return;
	}

	int oldViewportHeight = m_viewportHeight;
	m_viewportHeight = height;

	m_bottomLine = m_maxHeight - 1;	
	m_topLine = m_bottomLine - m_viewportHeight + 1;

	if(m_numLinesScrolledUp > 0)
	{
		int numLinesDifference = m_viewportHeight - oldViewportHeight;

		if(numLinesDifference > 0)
		{
			m_numLinesScrolledUp -= numLinesDifference;

			if(m_numLinesScrolledUp < 0)
			{
				m_numLinesScrolledUp = 0;
			}
		}
	}
	
}


string& TextManager::GetLine(int index)
{
	return m_text[index];
}

string& TextManager::GetLineAdjusted(int index)
{
	int actualLine = AdjustIndex(index);
	return m_text[actualLine];
}

void TextManager::SetLine(int index, string line)
{
	m_text[index] = line;
}

void TextManager::SetLineAdjusted(int index, string line)
{
	int actualLine = AdjustIndex(index);

	m_text[actualLine] = line;
}

void TextManager::SetCharAdjusted(int y, int x, char c)
{
	//wxLogDebug("SCA: y: %d, x: %d, c: %s");
	int actualLine = AdjustIndex(y);

	if( (actualLine > MAXHEIGHT) || (y >= m_viewportHeight))
	{
		wxLogDebug("Bad Y value in TM::GCA.  y = %d, viewport height = %d", y, m_viewportHeight);
		return;
	}
	m_text[actualLine][x] = c;
}

char TextManager::GetCharAdjusted(int y, int x)
{
	int actualLine = AdjustIndex(y);

	if( (actualLine > MAXHEIGHT) || (y >= m_viewportHeight))
	{
		wxLogDebug("Bad Y value in TM::GCA.  y = %d, viewport height = %d", y, m_viewportHeight);
		return ' ';
	}

	return m_text[actualLine][x];
}

unsigned short TextManager::GetColor(int y, int x)
{
	return m_color[y][x];
}

unsigned short TextManager::GetColorAdjusted(int y, int x)
{
	int actualLine = AdjustIndex(y);

	if( (actualLine > MAXHEIGHT) || (y >= m_viewportHeight))
	{
		wxLogDebug("Bad Y value in TM::GCA.  y = %d, viewport height = %d", y, m_viewportHeight);
		return 0;
	}
	return m_color[actualLine][x];
}

void TextManager::SetColor(int y, int x, unsigned short value)
{
	m_color[y][x] = value;
}

void TextManager::SetColorAdjusted(int y, int x, unsigned short value)
{
	int actualLine = AdjustIndex(y);

	if( (actualLine > MAXHEIGHT) || (y >= m_viewportHeight))
	{
		wxLogDebug("Bad Y value in TM::SCA.  y = %d, viewport height = %d", y, m_viewportHeight);
		return;
	}

	if(value != m_blankColor)
	{
		int q = 42;
	}
	m_color[actualLine][x] = value;
}

void TextManager::PrintToBitmap()
{
	wxColour colors[16];

	colors[0] = wxColour(0, 0, 0);                             // black
	colors[1] = wxColour(170, 0, 0);                           // red
	colors[2] = wxColour(0, 170, 0);                           // green
	colors[3] = wxColour(170, 0, 170);                         // yellow
	colors[4] = wxColour(0, 0, 170);                           // blue
	colors[5] = wxColour(170, 170, 0);                         // magenta
	colors[6] = wxColour(0, 170, 170);                         // cyan
	colors[7] = wxColour(192, 192, 192);                       // white
	colors[8] = wxColour(85, 85, 85);                          // bold black
	colors[9] = wxColour(255, 0, 0);                         // bold red
	colors[10] = wxColour(0, 255, 0);                        // bold green
	colors[11] = wxColour(255, 0, 255);                       // bold yellow
	colors[12] = wxColour(0, 0, 255);                        // bold blue
	colors[13] = wxColour(255, 255, 0);                       // bold magenta
	colors[14] = wxColour(0, 255, 255);                       // bold cyan
	colors[15] = wxColour(255, 255, 255); 

	wxMemoryDC dc;

	wxFont monospacedFont(10, wxMODERN, wxNORMAL, wxNORMAL, false, "Courier New");
	dc.SetFont(monospacedFont);

	long charWidth;
	long charHeight;

	dc.GetTextExtent("M", &charWidth, &charHeight);

	int bitmapWidth = charWidth * m_maxWidth;
	int bitmapHeight = charHeight * m_maxHeight;

	wxBitmap bm(bitmapWidth, bitmapHeight, 32);

	dc.SelectObject(bm);

	dc.SetBackgroundMode(wxSOLID);

	wxFile fout("c:\\temp\\characters.txt", wxFile::write);

	for(int y = 0; y < m_maxHeight; y++)
	{
		if(y == 95)
		{
			int q = 42;
		}
		wxString line;
		for(int x = 0; x < m_maxWidth; x++)
		{
			wxString character;
			char c = m_text[y][x];
			wxString str(c);

			unsigned short color = m_color[y][x];

			int bg_color;
			int fg_color;

			m_parent->DecodeColor(color, fg_color, bg_color);
			wxColour background = colors[bg_color];
			wxColour foreground = colors[fg_color];
			dc.SetTextBackground(background);
			dc.SetTextForeground(foreground);

			dc.DrawText(str, x * charWidth, y * charHeight);

			character.Printf("%s %2d %2d", str.c_str(), fg_color, bg_color);

			line += character;

			if(x != m_maxWidth - 1)
			{
				line += ", ";
			}

		}

		line += "\n";

		fout.Write(line);


	}

	fout.Close();

	
	wxImage image = bm.ConvertToImage();

	wxInitAllImageHandlers();

	image.SaveFile("c:\\temp\\textoutput.png", wxBITMAP_TYPE_PNG);
	

}

void TextManager::SetCursorLine(int line)
{
	//wxLogDebug("line: %d", line);
	if(line >= m_viewportHeight)
	{
		string bottomString = GetLineAdjusted(m_viewportHeight - 1);

		if(bottomString != m_blankline)
		{
			int numNewLines = line - m_viewportHeight + 1; 
			for(int i = 0; i < numNewLines; i++)
			{
				AddNewLine();
			}
		}
		

		line = m_viewportHeight - 1;		
	}
	else if(line < 0)
	{
		line = 0;
	}

	m_cursorLine = line;

}

void TextManager::CursorDown()
{
	string bottomLine = GetLineAdjusted(m_cursorLine);
	//wxLogDebug("CursorDown: %d/%d, line: %s", m_cursorLine, m_cursorLine + 1, bottomLine.c_str());

	if(m_cursorLine == m_viewportHeight - 1)
	{
		int q = 42;
	}
	SetCursorLine(m_cursorLine + 1);

	m_linesReceived++;
	
}

void TextManager::CursorUp()
{
	SetCursorLine(m_cursorLine - 1);
}

int TextManager::AdjustIndex(int index)
{
	int adjustedIndex = m_topLine + index - m_numLinesScrolledUp;
	return adjustedIndex;
}

int TextManager::GetNumLinesScrolled()
{
	return m_numLinesScrolledUp;
}

void TextManager::SetMaxSize(int newSize)
{
	if(newSize < m_viewportHeight)
	{
		return;
	}

	if(newSize < m_maxHeight)
	{
		int linesToPitch = m_maxHeight - newSize;

		for(int i = 0; i < linesToPitch; i++)
		{
			m_color.pop_front();
			m_text.pop_front();
		}
	}
}


int TextManager::GetLinesReceived()
{
	return m_linesReceived;
}