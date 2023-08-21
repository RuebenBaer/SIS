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

#ifndef __BASE_H
#define __BASE_H
#include "SIS_Kern.h"
#include "Dbl_Eingabe.h"
#include "Vektor/Vektor.h"

int BILDEINRUECKUNG = 100;

class MainApp: public wxApp
{
  public:
      virtual bool OnInit();
};

class SISFrame: public wxFrame
{
  public:
    SISFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    ~SISFrame();
    void OnQuit(wxCommandEvent &event);
    void OnRndRGBTextur(wxCommandEvent &event);
    void OnRndBWTextur(wxCommandEvent &event);
    void OnOpenTextur(wxCommandEvent &event);
    void OnOpenHoehenkarte(wxCommandEvent &event);
    void OnBatch(wxCommandEvent &event);
    void OnSaveStereogramm(wxCommandEvent &event);
    void OnSISAnzeigen(wxCommandEvent &event);
    void OnBlickModusWechseln(wxCommandEvent &event);
    void OnMehrfachTextur(wxCommandEvent &event);
    void OnDoppelBild(wxCommandEvent &event);
    void OnStartVerschieben(wxCommandEvent &event);
    void OnAnleitung(wxCommandEvent &event);
    void OnBelegung(wxCommandEvent &event);
    void OnLizenz(wxCommandEvent &event);
    void OnARDS(wxCommandEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnPaint(wxPaintEvent &event);
    void OnMouseWheel(wxMouseEvent &event);
    void OnKeyPress(wxKeyEvent &event);
    void OnKeyRelease(wxKeyEvent &event);
    void OnTexturZoom(wxCommandEvent &event);
    /*Maus*/
    void OnLeftDown(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    /*Focus*/
    void OnGotFocus(wxFocusEvent &event);
    void OnEinstellungenGeaendert(wxCommandEvent &event);
  private:
	/*Menues*/
	wxMenu *DateiMenu, *BearbeitenMenu, *InfoMenu, *TexturMenu;
	wxMenuItem *MenuPunktAnzeigen, *MenuPunktBlickModus, *MenuPunktMehrfachTextur, *MenuPunktDoppelbild;
	wxMenuBar *MenuBar;
	/*Bilder*/
    wxImage Leinwand, imgHoehenkarte, imgTextur;
    Vektor TexRelLage, TexAbsLage;
    SIS_Bild *SISTextur, *SISHoehenkarte, *SIStereogramm;
    SingleImageStereogram* tSIS;
    int DotsPerInch;
    wxFileDialog *HKOpener, *TexturOpener, *PictureSaver, *BatchOpener;
    float AnzeigeFaktor, ZoomFkt, Beschleuniger, TexturZoom, alterZoom;
    bool beimUeberblenden, zoomen, zoomenTextur, MehrfachTextur, StartVerschieben, SISangezeigt;
    /*Eingabe*/
    aruDblTxtCtrl *TXT_AugenAbstand, *TXT_DotsPerInch, *TXT_MattscheibenAbstand, *TXT_Verschub, *TXT_BildTiefe, *TXT_BildAbstand;
    /*Funktionen*/
    wxImage LeinwandBemalen(SIS_Bild*);
    void Ueberblenden(void);
    float AnzeigeSkalieren(wxDC&);
    bool LagePruefen(void);
    bool ParameterAnpassen(void);
    bool SISErzeugen(void);
    void HoehenKarteAufbringen();
    void TexturAufbringen();
    void RDTexturAnpassen(void);
	void ZufallsPunktBild(void);
    void Verschieben(int, int);
    void ZoomAnpassen(float&, bool);
    void TexturZoomAnpassen(float&, bool);
    void AnleitungOeffnen(void);
    void EingabefelderFuellen(void);
    /*Verschiedenes*/
    wxString tempTxt;
    wxPoint AlteMousePosition, NeueMousePosition;
    bool geaendert, keineNeuBerechnung;
    int StartPunkt;

      DECLARE_EVENT_TABLE()
};

enum
{
   ID_SIS_QUIT = wxID_HIGHEST+1, ID_SIS_TEXTUR_MENU, ID_SIS_OPEN_TEXTUR, ID_SIS_NEUE_RND_RGB_TEXTUR,
   ID_SIS_NEUE_RND_BW_TEXTUR, ID_SIS_OPEN_HK, ID_SIS_BATCH, ID_SIS_SAVE, ID_SIS_ERZEUGEN, ID_SIS_ANZEIGE,
   ID_SIS_AUGENABSTAND, ID_SIS_DOTSPERINCH, ID_SIS_MATTSCHEIBENABSTAND, ID_SIS_VERSCHUB, ID_SIS_BILDTIEFE,
   ID_SIS_BILDABSTAND, ID_SIS_MEHRFACHTEXTUR, ID_SIS_DOPPELBILD, ID_SIS_STARTPUNKT, ID_SIS_TEXTURZOOM,
   ID_SIS_ANLEITUNG, ID_SIS_BELEGUNG, ID_SIS_LIZENZ, ID_SIS_ARDS, ID_SIS_BLICKMODUS
};

enum TexturMode
{
	SIS_RND_RGB_TEXTUR = 0, SIS_RND_BW_TEXTUR, SIS_IMG_TEXTUR
}SIS_Textur_Mode;

#endif
