/*  SISerator - creates Single Image Stereograms from Heightmaps and Textures (both loaded from user) or Single Image Random Dot Stereograms from Heightmaps
    Copyright (C) 2015  Ansgar Rütten

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/
    
#ifndef __Anzeige_H
#define __Anzeige_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include "SIS_Kern.h"
#include<wx/image.h>

class AnzeigeFrame: public wxDialog
{
  public:
	AnzeigeFrame(wxWindow* parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size);
	~AnzeigeFrame();
	void OnEraseBackground(wxEraseEvent &event);
	void OnSize(wxSizeEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnMouseWheel(wxMouseEvent &event);
	void OnKeyPress(wxKeyEvent &event);
	void OnKeyRelease(wxKeyEvent &event);
	bool SetAnzeigeBild(SIS_Bild*);
	wxImage GetAnzeigeBild(void);
	void OnGotFocus(wxFocusEvent &event);
	void NeuZeichnen(void);
  private:
    wxImage Leinwand;
    float AnzeigeFaktor, ZoomFkt, Beschleuniger;
    bool zoomen;
    /*Funktionen*/
    /*float AnzeigeSkalieren(wxDC&);*/

      DECLARE_EVENT_TABLE()
};

enum
{
   ID_ANZ_QUIT = wxID_HIGHEST+1
};

#endif
