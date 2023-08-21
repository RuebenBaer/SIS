/* DONE (Ansi#1#): Pfad von wxString in char umwandeln (NEIN wxTextFile benutzt stattdessen)*/

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <fstream>
#include <cstring>
#include <wx/textfile.h>
#include "Textanzeige.h"
using namespace std;

/*Implementierung Textanzeige*/

BEGIN_EVENT_TABLE(TextAnzeige, wxFrame)
    EVT_SIZE(TextAnzeige::OnSize)
END_EVENT_TABLE()

TextAnzeige::TextAnzeige(wxString Pfad, wxString StatusText, wxWindow* parent, const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(parent, -1, title, pos, size, style)
{
    CreateStatusBar(1);
    SetStatusText(StatusText);
    Status = StatusText;

    TextFeld = new wxTextCtrl(this, 1, wxT("") , wxPoint(0,0), GetClientSize(), wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY);
    TextFeld->Clear();
    wxTextAttr t_Style = wxTextAttr(wxColour(0,0,0), wxColour(255,255,255), wxFont(11, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    TextFeld->SetDefaultStyle(t_Style);
    TextEinlesen(Pfad);
}

TextAnzeige::~TextAnzeige()
{
    delete TextFeld;
}

void TextAnzeige::TextEinlesen(wxString Pfad)
{
    wxTextFile* datei = new wxTextFile(Pfad);
    if(datei->Exists())datei->Open();
    if (datei->IsOpened())
    {
        int ZNr = 1;
        wxString zeile;
        for(zeile = datei->GetFirstLine(); !(datei->Eof()); zeile = datei->GetNextLine())
        {
            try{
                TextFeld->AppendText(zeile);
                TextFeld->AppendText(_("\n"));
                ZNr++;
            }
            catch(...){
                SetStatusText(wxString::Format(wxT("%i"), ZNr) + wxT(" - Fehler beim Lesen der Lizenzdatei"));
                return;
            }
        }
    SetStatusText(Status);
    } else {
        TextFeld->AppendText(wxString::FromAscii("Ein Fehler ist aufgetreten."));
    }
    datei->Close();
    delete datei;
    TextFeld->SetSelection(0L, 0L);
    Refresh();
    return;
}

void TextAnzeige::OnSize(wxSizeEvent& event)
{
    TextFeld->SetSize(GetClientSize());
    Refresh();
}
