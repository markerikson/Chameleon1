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

//////////////////////////////////////////////////////////////////////////////
///  public constructor TextManager
///  <TODO: insert text here>
///
///  @param  parent    GTerm * [=0] <TODO: insert text here>
///  @param  width     int     [=80] <TODO: insert text here>
///  @param  height    int     [=24] <TODO: insert text here>
///  @param  maxWidth  int     [=160] <TODO: insert text here>
///  @param  maxHeight int     [=100] <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
TextManager::TextManager(GTerm* parent, int width, int height, int maxWidth, int maxHeight /* = 50 */)
	:m_parent(parent), m_viewportWidth(width), m_viewportHeight(height), m_maxWidth(maxWidth), m_maxHeight(maxHeight)
{
	if(m_parent != NULL)
	{
		Reset();
	}
	
}

//////////////////////////////////////////////////////////////////////////////
///  public Reset
///  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public overloaded AddNewLine
///  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void TextManager::AddNewLine()
{
	AddNewLine(m_blankline);
}

//////////////////////////////////////////////////////////////////////////////
///  public overloaded AddNewLine
///  <TODO: insert text here>
///
///  @param  newline string  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-22-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public operator []
///  <TODO: insert text here>
///
///  @param  index    int  <TODO: insert text here>
///
///  @return string & <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
string &TextManager::operator [](int index)
{
	if(index != m_cursorLine)
	{
		//wxLogDebug("m_cursorLine: %d, index: %d", m_cursorLine, index);
	}
	return GetLineAdjusted(index);
}

//////////////////////////////////////////////////////////////////////////////
///  public Scroll
///  <TODO: insert text here>
///
///  @param  numLines int   <TODO: insert text here>
///  @param  scrollUp bool  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public GetSize
///  <TODO: insert text here>
///
///  @return int <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
int TextManager::GetSize()
{
	return m_text.size();
}

//////////////////////////////////////////////////////////////////////////////
///  public Resize
///  <TODO: insert text here>
///
///  @param  width  int  <TODO: insert text here>
///  @param  height int  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void TextManager::Resize(int width, int height)
{
	if(height == m_viewportHeight)
	{
		return;
	}

	int oldCursorLocation = m_topLine + m_cursorLine;

	int oldViewportHeight = m_viewportHeight;
	m_viewportHeight = height;

	m_bottomLine = m_maxHeight - 1;	
	m_topLine = m_bottomLine - m_viewportHeight + 1;

	//int newCursorLine;

	// TODO Need to do some more recalculations... this case here
	// is going to ripple off into stuff like SetCursorLine and so on
	// Hmm... maybe set m_topLine to the new cursor line and pop
	// off the bottom lines as necessary?
	if(m_topLine > oldCursorLocation)
	{

	}

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


//////////////////////////////////////////////////////////////////////////////
///  public GetLine
///  <TODO: insert text here>
///
///  @param  index    int  <TODO: insert text here>
///
///  @return string & <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
string& TextManager::GetLine(int index)
{
	return m_text[index];
}

//////////////////////////////////////////////////////////////////////////////
///  public GetLineAdjusted
///  <TODO: insert text here>
///
///  @param  index    int  <TODO: insert text here>
///
///  @return string & <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
string& TextManager::GetLineAdjusted(int index)
{
	int actualLine = AdjustIndex(index);
	return m_text[actualLine];
}

//////////////////////////////////////////////////////////////////////////////
///  public SetLine
///  <TODO: insert text here>
///
///  @param  index int     <TODO: insert text here>
///  @param  line  string  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void TextManager::SetLine(int index, string line)
{
	m_text[index] = line;
}

//////////////////////////////////////////////////////////////////////////////
///  public SetLineAdjusted
///  <TODO: insert text here>
///
///  @param  index int     <TODO: insert text here>
///  @param  line  string  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void TextManager::SetLineAdjusted(int index, string line)
{
	int actualLine = AdjustIndex(index);

	m_text[actualLine] = line;
}

//////////////////////////////////////////////////////////////////////////////
///  public SetCharAdjusted
///  <TODO: insert text here>
///
///  @param  y    int   <TODO: insert text here>
///  @param  x    int   <TODO: insert text here>
///  @param  c    char  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public GetCharAdjusted
///  <TODO: insert text here>
///
///  @param  y    int  <TODO: insert text here>
///  @param  x    int  <TODO: insert text here>
///
///  @return char <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public GetColor
///  <TODO: insert text here>
///
///  @param  y              int  <TODO: insert text here>
///  @param  x              int  <TODO: insert text here>
///
///  @return unsigned short <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
unsigned short TextManager::GetColor(int y, int x)
{
	return m_color[y][x];
}

//////////////////////////////////////////////////////////////////////////////
///  public GetColorAdjusted
///  <TODO: insert text here>
///
///  @param  y              int  <TODO: insert text here>
///  @param  x              int  <TODO: insert text here>
///
///  @return unsigned short <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public SetColor
///  <TODO: insert text here>
///
///  @param  y     int             <TODO: insert text here>
///  @param  x     int             <TODO: insert text here>
///  @param  value unsigned short  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void TextManager::SetColor(int y, int x, unsigned short value)
{
	m_color[y][x] = value;
}

//////////////////////////////////////////////////////////////////////////////
///  public SetColorAdjusted
///  <TODO: insert text here>
///
///  @param  y     int             <TODO: insert text here>
///  @param  x     int             <TODO: insert text here>
///  @param  value unsigned short  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public SetCursorLine
///  <TODO: insert text here>
///
///  @param  line int  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public CursorDown
///  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
///  public CursorUp
///  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
void TextManager::CursorUp()
{
	SetCursorLine(m_cursorLine - 1);
}

//////////////////////////////////////////////////////////////////////////////
///  private AdjustIndex
///  <TODO: insert text here>
///
///  @param  index int  <TODO: insert text here>
///
///  @return int   <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
int TextManager::AdjustIndex(int index)
{
	int adjustedIndex = m_topLine + index - m_numLinesScrolledUp;
	return adjustedIndex;
}

//////////////////////////////////////////////////////////////////////////////
///  public GetNumLinesScrolled
///  <TODO: insert text here>
///
///  @return int <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
int TextManager::GetNumLinesScrolled()
{
	return m_numLinesScrolledUp;
}

//////////////////////////////////////////////////////////////////////////////
///  public SetMaxSize
///  <TODO: insert text here>
///
///  @param  newSize int  <TODO: insert text here>
///
///  @return void
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////
///  public GetLinesReceived
///  <TODO: insert text here>
///
///  @return int <TODO: insert text here>
///
///  @author Mark Erikson @date 04-23-2004
//////////////////////////////////////////////////////////////////////////////
int TextManager::GetLinesReceived()
{
	return m_linesReceived;
}