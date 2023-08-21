/*  SISerator - creates Single Image Stereograms from Heightmaps and Textures (both loaded from user) or Single Image Random Dot Stereograms from Heightmaps
    Copyright (C) 2015  Ansgar R�tten

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

#ifndef __TXTANZEIGE_H
#define __TXTANZEIGE_H

class TextAnzeige: public wxFrame
{
  public:
    TextAnzeige(wxString Pfad, wxString StatusText, wxWindow* parent, const wxString &title, const wxPoint &pos, const wxSize &size, long);
    ~TextAnzeige();
    void OnSize(wxSizeEvent& event);
    void TextEinlesen(wxString);
  private:
    wxButton* Akzeptieren;
    wxButton* Ablehnen;
    wxTextCtrl* TextFeld;
    wxString Status;
    DECLARE_EVENT_TABLE()
};

#endif
