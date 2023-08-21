/* DONE (Ansi#1#): Dialog zur SIS Anzeige machen */
/* DONE (Ansi#2#): Einstellung über Fenster einrichten */
/* DONE (Ansi#3#): Einstellungen vor SIS Erzeugung übertragen*/
/* DONE (Ansi#3#): Ausrichtung Textur mit Maus und auf Schirm */
/* DONE (Ansi#2#): Hoehenfaktor relativieren*/
/* DONE (Ansi#1#): Ueberblenden implementieren (Algorithmus)*/
/* DONE (Ansi#1#): Ueberblenden implementieren (Einstellung)*/
/* TODO (Ansi#1#): HK und Textur bei Programmbeginn laden (Beispiele)*/
/* TODO (Ansi#2#): verkleinerte Arbeitsversionen von HK und Textur*/
/* TODO (Ansi#3#): Zoomfaktor für Textur*/

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

#include <wx/string.h>
#include <wx/imagjpeg.h>
#include <wx/dcbuffer.h>
#include "base.h"
#include "SIS_Kern.h"
#include <cmath>
#include "ErrorObjekt.h"
#include <cstring>
#include "TextAnzeige.h"

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
   SISFrame *win = new SISFrame(_("Ste reogr amme Stereo gramme"), wxPoint (100, 100), wxSize(600, 600));
   //wxMessageDialog(win, _("SISFrame erzeugt"), wxT("Test")).ShowModal();
   win->Show(TRUE);
   SetTopWindow(win);
   //wxMessageDialog(win, _("SISFrame angezeigt"), wxT("Test")).ShowModal();

   return TRUE;
}


BEGIN_EVENT_TABLE(SISFrame, wxFrame)
   EVT_MENU(ID_SIS_QUIT, SISFrame::OnQuit)
   EVT_MENU(ID_SIS_OPEN_TEXTUR, SISFrame::OnOpenTextur)
   EVT_MENU(ID_SIS_NEUE_RND_RGB_TEXTUR, SISFrame::OnRndRGBTextur)
   EVT_MENU(ID_SIS_NEUE_RND_BW_TEXTUR, SISFrame::OnRndBWTextur)
   EVT_MENU(ID_SIS_OPEN_HK, SISFrame::OnOpenHoehenkarte)
   EVT_MENU(ID_SIS_BATCH, SISFrame::OnBatch)
   EVT_MENU(ID_SIS_SAVE, SISFrame::OnSaveStereogramm)
   EVT_MENU(ID_SIS_ERZEUGEN, SISFrame::OnSISAnzeigen)
   EVT_MENU(ID_SIS_BLICKMODUS, SISFrame::OnBlickModusWechseln)
   EVT_MENU(ID_SIS_STARTPUNKT, SISFrame::OnStartVerschieben)
   EVT_MENU(ID_SIS_MEHRFACHTEXTUR, SISFrame::OnMehrfachTextur)
   EVT_MENU(ID_SIS_DOPPELBILD, SISFrame::OnDoppelBild)
   EVT_MENU(ID_SIS_TEXTURZOOM, SISFrame::OnTexturZoom)
   EVT_MENU(ID_SIS_ANLEITUNG, SISFrame::OnAnleitung)
   EVT_MENU(ID_SIS_BELEGUNG, SISFrame::OnBelegung)
   EVT_MENU(ID_SIS_LIZENZ, SISFrame::OnLizenz)
   EVT_MENU(ID_SIS_ARDS, SISFrame::OnARDS)
   EVT_SIZE(SISFrame::OnSize)
   EVT_PAINT(SISFrame::OnPaint)
   EVT_MOUSEWHEEL(SISFrame::OnMouseWheel)
   EVT_MOUSE_EVENTS(SISFrame::OnMouseMove)
   EVT_LEFT_DOWN(SISFrame::OnLeftDown)
   EVT_LEFT_UP(SISFrame::OnLeftUp)
   EVT_KEY_DOWN(SISFrame::OnKeyPress)
   EVT_KEY_UP(SISFrame::OnKeyRelease)
   EVT_ERASE_BACKGROUND(SISFrame::OnEraseBackground)
   EVT_SET_FOCUS(SISFrame::OnGotFocus)
   EVT_TEXT_ENTER(ID_SIS_AUGENABSTAND, SISFrame::OnEinstellungenGeaendert)
   EVT_TEXT_ENTER(ID_SIS_DOTSPERINCH, SISFrame::OnEinstellungenGeaendert)
   EVT_TEXT_ENTER(ID_SIS_MATTSCHEIBENABSTAND, SISFrame::OnEinstellungenGeaendert)
   EVT_TEXT_ENTER(ID_SIS_VERSCHUB, SISFrame::OnEinstellungenGeaendert)
   EVT_TEXT_ENTER(ID_SIS_BILDTIEFE, SISFrame::OnEinstellungenGeaendert)
   EVT_TEXT_ENTER(ID_SIS_BILDABSTAND, SISFrame::OnEinstellungenGeaendert)
END_EVENT_TABLE()

SISFrame::SISFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
    CreateStatusBar(2);

    DateiMenu = new wxMenu;
    BearbeitenMenu = new wxMenu;
    InfoMenu = new wxMenu;
    TexturMenu = new wxMenu;
    MenuBar = new wxMenuBar;

    TexturMenu->Append(ID_SIS_OPEN_TEXTUR, _("&Textur laden (Bilddatei)"));
    TexturMenu->Append(ID_SIS_NEUE_RND_RGB_TEXTUR, _("&Bunte Zufallstextur"));
    TexturMenu->Append(ID_SIS_NEUE_RND_BW_TEXTUR, _("&Graue Zufallstextur"));

	DateiMenu->Append(ID_SIS_TEXTUR_MENU, _("&Textur"), TexturMenu);
    DateiMenu->Append(ID_SIS_OPEN_HK, _("&Höhenkarte laden"));
    DateiMenu->Append(ID_SIS_BATCH, _("Stapel&verarbeitung"), _("Lädt mehrere Bilder und wandelt diese"));
    DateiMenu->AppendSeparator();
    DateiMenu->Append(ID_SIS_SAVE, _("Stereogramm &Speichern"));
    DateiMenu->AppendSeparator();
    DateiMenu->Append(ID_SIS_QUIT, _("&Beenden"));

    MenuPunktAnzeigen = new wxMenuItem(BearbeitenMenu, ID_SIS_ERZEUGEN, _("&SIS anzeigen"), wxT(""), wxITEM_CHECK);
    BearbeitenMenu->Append(MenuPunktAnzeigen);
    MenuPunktBlickModus = new wxMenuItem(BearbeitenMenu, ID_SIS_BLICKMODUS, _("Zum &Kreuzblick wechseln"), wxT(""));
    BearbeitenMenu->Append(MenuPunktBlickModus);
    BearbeitenMenu->AppendSeparator();
    BearbeitenMenu->Append(ID_SIS_TEXTURZOOM, _("Textur &zoomen"), wxT(""), wxITEM_CHECK);
    BearbeitenMenu->AppendSeparator();
    BearbeitenMenu->Append(ID_SIS_STARTPUNKT, _("Startpunkt &verschieben"), wxT(""), wxITEM_CHECK);
    MenuPunktMehrfachTextur = new wxMenuItem(BearbeitenMenu, ID_SIS_MEHRFACHTEXTUR, _("&Mehrfach texturieren"), wxT(""), wxITEM_CHECK);
    BearbeitenMenu->Append(MenuPunktMehrfachTextur);
    MenuPunktDoppelbild = new wxMenuItem(BearbeitenMenu, ID_SIS_DOPPELBILD, _("&Doppelbild"), wxT(""), wxITEM_CHECK);
    BearbeitenMenu->Append(MenuPunktDoppelbild);

    InfoMenu->Append(ID_SIS_ANLEITUNG, _("&Anleitung"), _("Übersicht über die Funktionen des Programms"));
    InfoMenu->Append(ID_SIS_BELEGUNG, _("&Tastaturbelegungen"), _("Zeigt die Belegung der Tastatur und Maustasten an"));
    InfoMenu->Append(ID_SIS_ARDS, _("Die Theorie der SIS"), _("Zeigt tolle Infos an"));
    InfoMenu->AppendSeparator();
    InfoMenu->Append(ID_SIS_LIZENZ, _("&Lizenz"), _("GNU GENERAL PUBLIC LICENSE"));

    MenuBar->Append(DateiMenu, _("&Datei"));
    MenuBar->Append(BearbeitenMenu, _("&Bearbeiten"));
    MenuBar->Append(InfoMenu, _("&Hilfe"));
    SetMenuBar(MenuBar);

    wxImageHandler *JPEGHandler = new wxJPEGHandler();
    wxImageHandler *PNGHandler = new wxPNGHandler();
    wxImage::AddHandler(JPEGHandler);
    wxImage::AddHandler(PNGHandler);

    HKOpener = new wxFileDialog(this, _("Höhenkarte öffnen"),wxT("Beispielbilder/3D.png"),wxT(""),wxT("PNG/JPEG-Dateien (*.png, *.jpg)|*png; *.jpg"),1);
    TexturOpener = new wxFileDialog(this, _("Textur öffnen"),wxT(""),wxT(""),wxT("PNG/JPEG-Dateien (*.png, *.jpg)|*png; *.jpg"),1);
    PictureSaver = new wxFileDialog(this, wxT("Stereogramm speichern"),wxT(""),wxT(""),wxT("PNG-Dateien (*.png)|*.png|JPEG-Dateien (*.jpg)|*.jpg"),
										wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	BatchOpener = new wxFileDialog(this, _("Stapelverarbeitung - Auswahl der Höhenkarten")
										,wxT(""),wxT(""),wxT("PNG-Dateien (*.png)|*.png|JPEG-Dateien (*.jpg)|*.jpg"), wxFD_MULTIPLE);

	/*SIS Vorbereitungen*/
	try
	{
		SISTextur = new SIS_Bild(1,1);
		SISHoehenkarte = new SIS_Bild(1,1);
		SIStereogramm = new SIS_Bild(1,1);
		tSIS = new SIS_Einfach(SISHoehenkarte, SISTextur);
	}
	catch(Error* e)
	{
		wxMessageDialog(this, _(e->message()), _("Fehler")).ShowModal();
		//delete e;
	}
	StartPunkt = BILDEINRUECKUNG;
	tSIS->SetzeStartPunkt(0);

	/*Bildanzeige*/
	Leinwand = wxImage(100,100);
	ZoomFkt = 1.0;
	TexturZoom = 1.0;
	zoomen = 0;
	Leinwand = LeinwandBemalen(SISHoehenkarte);
	TexRelLage.SetKoordinaten(0,0,0);
	TexAbsLage.SetKoordinaten(0,0,0);
	SIS_Textur_Mode = SIS_IMG_TEXTUR;

	/*Eingabefelder vorbereiten*/
	wxColor TxtColor =  wxColor(200,200,200);
	wxColor back = GetBackgroundColour();
	char Zeile = 0;
	DotsPerInch = 96;
	/*Augenabstand*/
	wxTextCtrl* NAME_AugenAbstand = new wxTextCtrl(this, -1, _("Augenabstand"), wxPoint(5, (Zeile++ * 20 + 10)),
										wxSize(90, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("NAME_AugenAbstand"));
	NAME_AugenAbstand->SetBackgroundColour(back);
	TXT_AugenAbstand = new aruDblTxtCtrl(this, ID_SIS_AUGENABSTAND, _(""), wxPoint(5, (Zeile * 20 + 5)),
										wxSize(45, 20), wxTE_PROCESS_ENTER|wxTE_RIGHT|wxBORDER_SIMPLE|wxTE_NOHIDESEL,
										wxDefaultValidator, _("TXT_AugenAbstand"));
	TXT_AugenAbstand->SetBackgroundColour(TxtColor);

	wxTextCtrl* DIM_AugenAbstand = new wxTextCtrl(this, -1, _("cm"), wxPoint(55, (Zeile++ * 20 + 7)),
										wxSize(30, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("DIM_AugenAbstand"));
	DIM_AugenAbstand->SetBackgroundColour(back);
    /*ENDE Augenabstand*/

	/*Dots per inch*/
	wxTextCtrl* NAME_DotsPerInch = new wxTextCtrl(this, -1, _("Auflösung"), wxPoint(5, (Zeile++ * 20 + 10)),
										wxSize(90, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("TXT_DotsPerInch"));
	NAME_DotsPerInch->SetBackgroundColour(back);
	TXT_DotsPerInch = new aruDblTxtCtrl(this, ID_SIS_DOTSPERINCH, _(""), wxPoint(5, (Zeile * 20 + 5)),
										wxSize(45, 20), wxTE_PROCESS_ENTER|wxTE_RIGHT|wxSIMPLE_BORDER|wxTE_NOHIDESEL,
										wxDefaultValidator, _("TXT_DotsPerInch"));
	TXT_DotsPerInch->SetBackgroundColour(TxtColor);

	wxTextCtrl* DIM_DotsPerInch = new wxTextCtrl(this, -1, _("dpi"), wxPoint(55, (Zeile++ * 20 + 7)),
										wxSize(30, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("DIM_DotsPerInch"));
	DIM_DotsPerInch->SetBackgroundColour(back);
	/*ENDE Dots per inch*/

	/*Mattscheibenabstand*/
	wxTextCtrl* NAME_MattscheibenAbstand = new wxTextCtrl(this, -1, _("Entfernung Bild"), wxPoint(5, (Zeile++ * 20 + 10)),
										wxSize(90, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("NAME_MattscheibenAbstand"));
	NAME_MattscheibenAbstand->SetBackgroundColour(back);
	TXT_MattscheibenAbstand = new aruDblTxtCtrl(this, ID_SIS_MATTSCHEIBENABSTAND, _(""), wxPoint(5, (Zeile * 20 + 5)),
										wxSize(45, 20), wxTE_PROCESS_ENTER|wxTE_RIGHT|wxSIMPLE_BORDER|wxTE_NOHIDESEL,
										wxDefaultValidator, _("TXT_MattscheibenAbstand"));
	TXT_MattscheibenAbstand->SetBackgroundColour(TxtColor);

	wxTextCtrl* DIM_MattscheibenAbstand = new wxTextCtrl(this, -1, _("cm"), wxPoint(55, (Zeile++ * 20 + 7)),
										wxSize(30, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("DIM_MattscheibenAbstand"));
	DIM_MattscheibenAbstand->SetBackgroundColour(back);
	/*ENDE Mattscheibenabstand*/

	/*Verschub*/
	wxTextCtrl* NAME_Verschub = new wxTextCtrl(this, -1, _("Verschub"), wxPoint(5, (Zeile++ * 20 + 10)),
										wxSize(90, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("NAME_Verschub"));
	NAME_Verschub->SetBackgroundColour(back);
	TXT_Verschub = new aruDblTxtCtrl(this, ID_SIS_VERSCHUB, _(""), wxPoint(5, (Zeile * 20 + 5)),
										wxSize(45, 20), wxTE_PROCESS_ENTER|wxTE_RIGHT|wxSIMPLE_BORDER|wxTE_NOHIDESEL,
										wxDefaultValidator, _("TXT_Verschub"));
	TXT_Verschub->SetBackgroundColour(TxtColor);

	wxTextCtrl* DIM_Verschub = new wxTextCtrl(this, -1, _("cm"), wxPoint(55, (Zeile++ * 20 + 7)),
										wxSize(30, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("DIM_Verschub"));
	DIM_Verschub->SetBackgroundColour(back);
	/*ENDE Verschub*/

	/*Hoehenfaktor*/
	wxTextCtrl* NAME_Faktor = new wxTextCtrl(this, -1, _("Bildtiefe"), wxPoint(5, (Zeile++ * 20 + 10)),
										wxSize(90, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("NAME_Faktor"));
	NAME_Faktor->SetBackgroundColour(back);
	TXT_BildTiefe = new aruDblTxtCtrl(this, ID_SIS_BILDTIEFE, _(""), wxPoint(5, (Zeile * 20 + 5)),
										wxSize(45, 20), wxTE_PROCESS_ENTER|wxTE_RIGHT|wxSIMPLE_BORDER|wxTE_NOHIDESEL,
										wxDefaultValidator, _("TXT_BildTiefe"));
	TXT_BildTiefe->SetBackgroundColour(TxtColor);

	wxTextCtrl* DIM_Faktor = new wxTextCtrl(this, -1, _("cm"), wxPoint(55, (Zeile++ * 20 + 7)),
										wxSize(30, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("DIM_Faktor"));
	DIM_Faktor->SetBackgroundColour(back);
	/*ENDE Hoehenfaktor*/

	/*Bildabstand*/
	wxTextCtrl* NAME_BildAbstand = new wxTextCtrl(this, -1, _("Bildabstand"), wxPoint(5, (Zeile++ * 20 + 10)),
										wxSize(90, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("NAME_BildAbstand"));
	NAME_BildAbstand->SetBackgroundColour(back);
	TXT_BildAbstand = new aruDblTxtCtrl(this, ID_SIS_BILDABSTAND, _(""), wxPoint(5, (Zeile * 20 + 5)),
										wxSize(45, 20), wxTE_READONLY|wxTE_RIGHT|wxNO_BORDER,
										wxDefaultValidator, _("TXT_BildAbstand"));
	TXT_BildAbstand->SetBackgroundColour(back);

	wxTextCtrl* DIM_BildAbstand = new wxTextCtrl(this, -1, _("cm"), wxPoint(55, (Zeile++ * 20 + 5)),
										wxSize(30, 15), wxTE_READONLY|wxTE_LEFT|wxNO_BORDER,
										wxDefaultValidator, _("DIM_BildAbstand"));
	DIM_BildAbstand->SetBackgroundColour(back);
	/*ENDE Bildabstand*/

	TXT_AugenAbstand->SetVorgaenger(TXT_BildTiefe);
	TXT_AugenAbstand->SetNachfolger(TXT_DotsPerInch);

	TXT_DotsPerInch->SetVorgaenger(TXT_AugenAbstand);
	TXT_DotsPerInch->SetNachfolger(TXT_MattscheibenAbstand);

	TXT_MattscheibenAbstand->SetVorgaenger(TXT_DotsPerInch);
	TXT_MattscheibenAbstand->SetNachfolger(TXT_Verschub);

	TXT_Verschub->SetVorgaenger(TXT_MattscheibenAbstand);
	TXT_Verschub->SetNachfolger(TXT_BildTiefe);

	TXT_BildTiefe->SetVorgaenger(TXT_Verschub);
	TXT_BildTiefe->SetNachfolger(TXT_AugenAbstand);

	EingabefelderFuellen();

    imgTextur.LoadFile(wxT("Beispielbilder/Textur.png"),wxBITMAP_TYPE_PNG);
    TexturAufbringen();
    imgHoehenkarte.LoadFile(wxT("Beispielbilder/3D.png"),wxBITMAP_TYPE_PNG);
    HoehenKarteAufbringen();

	beimUeberblenden = 0;
	StartVerschieben = 0;

	Beschleuniger = 1.0;
	StartPunkt = 0;

    SetStatusText(_("SIS SIS  SIS SIS  SIS SIS"),0);

    /*Icon laden*/
    SetIcon(wxIcon(wxT("source/Bilder/SISsy.ico"), wxBITMAP_TYPE_ICO));

    /*Abfragen setzen*/
    geaendert=1;
    keineNeuBerechnung = 0;
    SISangezeigt = 0;
    Refresh();
}

SISFrame::~SISFrame()
{
	delete SISTextur;
	delete SISHoehenkarte;
	delete tSIS;
	//delete SIStereogramm;
    //wxMessageDialog(this, "~SISFrame", "Ende").ShowModal();
}

void SISFrame::OnQuit(wxCommandEvent & WXUNUSED(event))
{
    Close(TRUE);
}

void SISFrame::OnSize(wxSizeEvent& event)
{
    return;
}

void SISFrame::OnOpenTextur(wxCommandEvent &event)
{
    if(TexturOpener->ShowModal()==wxID_CANCEL)return;
    wxString DateiEndung, Pfad;
    DateiEndung = Pfad = TexturOpener->GetPath();
    //wxMessageDialog(this, Pfad, wxT("Test")).ShowModal();
    TexturZoom = 1.0;
    TexAbsLage = Vektor(0.0, 0.0, 0.0);
    TexRelLage = TexAbsLage;

    /*Endung suchen*/
    for(int i= DateiEndung.Len(); i>0;i--)
    {
		if(DateiEndung.GetChar(i) == '.')
		{
			DateiEndung = Pfad.Right(DateiEndung.Len()-i-1);
			break;
		}
	}

	if(!DateiEndung.CmpNoCase(wxT("jpg")) || !DateiEndung.CmpNoCase(wxT("jpeg")))
	{
		imgTextur.LoadFile(Pfad, wxBITMAP_TYPE_JPEG);
		SetStatusText(_("JPEG geöffnet"));
	}else
	if(!DateiEndung.CmpNoCase(wxT("png")))
	{
		imgTextur.LoadFile(Pfad, wxBITMAP_TYPE_PNG);
		SetStatusText(_("PNG geöffnet"));
	}else{
		SetStatusText(_("Dateityp wird nicht unterstützt"));
		return;
	}
	SetStatusText(Pfad,1);
	SIS_Textur_Mode = SIS_IMG_TEXTUR;
	TexturAufbringen();
	geaendert=1;
    Refresh();
    return;
}

void SISFrame::OnRndRGBTextur(wxCommandEvent &event)
{
	if(imgTextur.Ok())
	{
        unsigned char* tempDaten = imgTextur.GetData();
    	srand(time(NULL));
    	unsigned char Wert;
    	for(int i = 0; i<(imgTextur.GetHeight()*imgTextur.GetWidth()*3);i++)
    	{
    		Wert = rand()%255;
    		tempDaten[i]=Wert;
    	}
    }
	SIS_Textur_Mode = SIS_RND_RGB_TEXTUR;
    TexturZoom = 1.0;
    StartPunkt = 0;
	TexturAufbringen();
	geaendert=1;
    Refresh();
	return;
}

void SISFrame::OnRndBWTextur(wxCommandEvent &event)
{
    if(imgTextur.Ok())
	{
        unsigned char* tempDaten = imgTextur.GetData();
    	srand(time(NULL));
    	unsigned char Wert;
    	for(int i = 0; i<(imgTextur.GetHeight()*imgTextur.GetWidth()*3);i+=3)
    	{
    		Wert = rand()%255;
    		tempDaten[i]=tempDaten[i+1]=tempDaten[i+2]=Wert;
    	}
	}
	SIS_Textur_Mode = SIS_RND_BW_TEXTUR;
    TexturZoom = 1.0;
    StartPunkt = 0;
	TexturAufbringen();
	geaendert=1;
    Refresh();
	return;
}

void SISFrame::OnOpenHoehenkarte(wxCommandEvent &event)
{
    if(HKOpener->ShowModal()==wxID_CANCEL)return;
    wxString DateiEndung, Pfad;
    DateiEndung = Pfad = HKOpener->GetPath();
	TexturZoom = 1.0;
	StartPunkt = 0;
    TexAbsLage = Vektor(0.0, 0.0, 0.0);
    TexRelLage = TexAbsLage;

	/*Endung suchen*/
    for(int i= DateiEndung.Len(); i>0;i--)
    {
		if(DateiEndung.GetChar(i) == '.')
		{
			DateiEndung = Pfad.Right(DateiEndung.Len()-i-1);
			break;
		}
	}

	if(!DateiEndung.CmpNoCase(wxT("jpg")) || !DateiEndung.CmpNoCase(wxT("jpeg")))
	{
		imgHoehenkarte.LoadFile(Pfad, wxBITMAP_TYPE_JPEG);
		SetStatusText(_("JPEG geöffnet"));
	}else
	if(!DateiEndung.CmpNoCase(wxT("png")))
	{
		imgHoehenkarte.LoadFile(Pfad, wxBITMAP_TYPE_PNG);
		SetStatusText(_("PNG geöffnet"));
	}else{
		SetStatusText(_("Dateityp wird nicht unterstützt"));
		return;
	}

    HoehenKarteAufbringen();
	RDTexturAnpassen();
	geaendert=1;
	Refresh();
	return;
}

void SISFrame::OnBatch(wxCommandEvent &event)
{
    keineNeuBerechnung=1;
    if(BatchOpener->ShowModal()==wxID_CANCEL)return;
    if(PictureSaver->ShowModal()==wxID_CANCEL)return;
    wxArrayString DateiListe;
	BatchOpener->GetPaths(DateiListe);
	if(!(DateiListe.IsEmpty()))
	{
		if(!ParameterAnpassen())
		{
			SetStatusText(wxT("Stapelverarbeitung abgebrochen"));
			SetStatusText(_("Einstellungen fehlerhaft"),1);
			return;
		}
		SetStatusText(_("Stapelverarbeitung"), 0);
		for(unsigned int i = 0; i < DateiListe.GetCount(); i++)
		{
			wxString aktuelleDatei = DateiListe.Item(i);
			wxString SpeicherPfad, DateiEndung;

            DateiEndung = aktuelleDatei;

			/*Endung suchen*/
            for(int j= DateiEndung.Len(); j>0;j--)
            {
                if(DateiEndung.GetChar(j) == '.')
                {
                    DateiEndung = aktuelleDatei.Right(DateiEndung.Len()-j-1);
                    break;
                }
            }

            if(!DateiEndung.CmpNoCase(wxT("jpg")) || !DateiEndung.CmpNoCase(wxT("jpeg")))
            {
                imgHoehenkarte.LoadFile(aktuelleDatei, wxBITMAP_TYPE_JPEG);
                SetStatusText(_("JPEG geöffnet"));
            }else
            if(!DateiEndung.CmpNoCase(wxT("png")))
            {
                imgHoehenkarte.LoadFile(aktuelleDatei, wxBITMAP_TYPE_PNG);
                SetStatusText(_("PNG geöffnet"));
            }else{
                SetStatusText("Dateityp " + DateiEndung + " wird nicht unterstützt");
                return;
            }
			SetStatusText(_("Verarbeite ") + DateiListe.Item(i), 1);
			HoehenKarteAufbringen();
			if(SIS_Textur_Mode != SIS_IMG_TEXTUR)
            {
                tSIS->SetzeTexturBreitenNullage(i*5);
                tSIS->SetzeTexturHoehenNullage(i*5);
            }

			wxString Pfad = PictureSaver->GetPath();
			for(int j= Pfad.Len(); j>0;j--)
			{
				if(Pfad.GetChar(j) == '.')
				{
					SpeicherPfad = Pfad.Left(j) + wxString::Format(wxT("%i"), i) + Pfad.Right(Pfad.Len()-j);
					break;
				}
			}

			wxImage tempLeinwand;
			SIS_Bild* tempSIS;
			try
			{
			    tempSIS = tSIS->HoleStereogramm();
			}
			catch(Error* e)
			{
				wxMessageDialog(this, _(e->message()), _("Fehler bei Erzeugung")).ShowModal();
				//delete e;
				SetStatusText(wxT("Erzeugung abgebrochen"));
				return;
			}
            tempLeinwand = LeinwandBemalen(tempSIS);

			bool Erfolg;
			if(PictureSaver->GetWildcard() == wxT(".png"))
			{
				Erfolg = tempLeinwand.SaveFile(SpeicherPfad, wxBITMAP_TYPE_PNG);
			}else{
				Erfolg = tempLeinwand.SaveFile(SpeicherPfad, wxBITMAP_TYPE_JPEG);
			}
			if(!Erfolg)
			{
				SetStatusText(_("Bild wurde nicht gespeichert"));
			}else{
				SetStatusText(_("Bild erfolgreich gespeichert"));
			}
		}
	}
	SetStatusText(wxT("Stapelverarbeitung abgeschlossen"));
	SetStatusText(wxT(""),1);
	keineNeuBerechnung=0;
    return;
}

void SISFrame::OnSaveStereogramm(wxCommandEvent &event)
{
	bool Erfolg;
	wxImage tempLeinwand;

	if(PictureSaver->ShowModal()==wxID_CANCEL)return;
	float tempZoom = ZoomFkt;
    ZoomFkt = 1.0;
    HoehenKarteAufbringen();
	TexturAufbringen();
	if(SISErzeugen())
    {
        tempLeinwand = LeinwandBemalen(SIStereogramm);
    }else{
		SetStatusText("Bild wurde nicht gespeichert");
		SetStatusText("Fehler beim Erzeugen", 1);
    }

	wxString DateiEndung = PictureSaver->GetPath();
    /*Endung suchen*/
    for(int i= DateiEndung.Len(); i>0; i--)
    {
		if(DateiEndung.GetChar(i) == '.')
		{
			DateiEndung = PictureSaver->GetPath().Right(DateiEndung.Len()-i-1);
			//wxMessageDialog(this, _("Die Endung lautet '")+DateiEndung+wxT("'"), wxT("Test")).ShowModal();
			break;
		}
	}

	Erfolg = false;

	if(!DateiEndung.CmpNoCase(wxT("jpg")) || !DateiEndung.CmpNoCase(wxT("jpeg")))
	{
		Erfolg = tempLeinwand.SaveFile(PictureSaver->GetPath(), wxBITMAP_TYPE_JPEG);
	}
	if(!DateiEndung.CmpNoCase(wxT("png")))
	{
		Erfolg = tempLeinwand.SaveFile(PictureSaver->GetPath(), wxBITMAP_TYPE_PNG);
	}

	if(!Erfolg)
	{
		SetStatusText(_("Bild wurde nicht gespeichert"));
	}else{
		SetStatusText(_("Bild erfolgreich gespeichert"));
	}
	ZoomFkt = tempZoom;
	return;
}

void SISFrame::OnSISAnzeigen(wxCommandEvent &event)
{
    if(MenuPunktAnzeigen->IsChecked())
    {
        SISangezeigt = 1;
    }else{
        SISangezeigt = 0;
    }
    Refresh();
    return;
}

void SISFrame::OnBlickModusWechseln(wxCommandEvent &event)
{
    if(tSIS->KreuzBlick())
    {
        tSIS->KreuzBlick(false);
        MenuPunktBlickModus->SetItemLabel(_("Zum &Kreuzblick wechseln"));
    }else{
        tSIS->KreuzBlick(true);
        MenuPunktBlickModus->SetItemLabel(_("Zum &Parallelblick wechseln"));
    }
    EingabefelderFuellen();
    geaendert = 1;
    Refresh();
    return;
}

void SISFrame::OnPaint(wxPaintEvent &event)
{
   	wxBufferedPaintDC dc(this);
   	if(SISangezeigt)
   	{
   	    if(keineNeuBerechnung)
        {
            Leinwand = Leinwand.Rescale(Leinwand.GetWidth()*(ZoomFkt/alterZoom),
                                        Leinwand.GetHeight()*(ZoomFkt/alterZoom),
                                        wxIMAGE_QUALITY_HIGH);
            alterZoom = ZoomFkt;
        }else{
            if(SISErzeugen())
            {
                Leinwand = LeinwandBemalen(SIStereogramm);
            }else{
                SetStatusText("SIS kann nicht angezeigt werden");
                SetStatusText("Fehler beim Erzeugen", 1);
                Ueberblenden();
            }
        }
    }else{
        Ueberblenden();
    }
    if(Leinwand.Ok())
	{
		/*Hintergrund zeichnen*/
		wxRect rect(wxPoint(0, 0), GetClientSize());
		wxColour back = GetBackgroundColour();
		dc.SetBrush(wxBrush(back));
		dc.SetPen(wxPen(back, 1));
		dc.DrawRectangle(rect);
		/*Hintergrund Ende*/
		/*int tempBreite = int(Leinwand.GetWidth());
		int tempHoehe = int(Leinwand.GetHeight());*/
		dc.DrawBitmap(wxBitmap(Leinwand), wxPoint(BILDEINRUECKUNG, 0), false);
	}
	if(!(MenuPunktAnzeigen->IsChecked()))
    {
    	/*Startpunkt Linieren*/
    	wxColour Linie(180, 30, 30);
        dc.SetBrush(wxBrush(Linie));
        dc.SetPen(wxPen(Linie, 2));
        /*dc.DrawLine(wxCoord (StartPunkt*ZoomFkt+BILDEINRUECKUNG), wxCoord (0),
                wxCoord (StartPunkt*ZoomFkt+BILDEINRUECKUNG), wxCoord (SISHoehenkarte->Hoehe()*ZoomFkt));*/
        dc.DrawLine(wxCoord (StartPunkt+BILDEINRUECKUNG), wxCoord (0),
            wxCoord (StartPunkt+BILDEINRUECKUNG), wxCoord (SISHoehenkarte->Hoehe()));
    }
   	return;
}
void SISFrame::OnDoppelBild(wxCommandEvent& event)
{
    if(event.IsChecked())
    {
        MenuPunktMehrfachTextur->Check(0);
        geaendert = 1;
        delete tSIS;
        tSIS = new SIS_Doppelbild(SISHoehenkarte, SISTextur);
        tSIS->SetzeStartPunkt((int)(StartPunkt));
    }else{
        geaendert = 1;
        delete tSIS;
        tSIS = new SIS_Einfach(SISHoehenkarte, SISTextur);
        tSIS->SetzeStartPunkt((int)(StartPunkt));
	}
    Refresh();
    return;
}

void SISFrame::OnMehrfachTextur(wxCommandEvent& event)
{
    if(event.IsChecked())
    {
        MenuPunktDoppelbild->Check(0);
        geaendert = 1;
        delete tSIS;
        tSIS = new SIS_Mehrfach(SISHoehenkarte, SISTextur);
        tSIS->SetzeStartPunkt((int)(StartPunkt));
    }else{
        geaendert = 1;
        delete tSIS;
        tSIS = new SIS_Einfach(SISHoehenkarte, SISTextur);
        tSIS->SetzeStartPunkt((int)(StartPunkt));
	}
    Refresh();
    return;
}

void SISFrame::OnTexturZoom(wxCommandEvent& event)
{
    if(event.IsChecked())
    {
		zoomenTextur = 1;
    }else{
		zoomenTextur = 0;
	}
    Refresh();
    return;
}

void SISFrame::OnGotFocus(wxFocusEvent &event)
{
    //Refresh();
	return;
}

bool SISFrame::SISErzeugen(void)
{
    geaendert = 0;

    SetStatusText(wxT("Stereogramm neu anlegen"));
    SetStatusText(wxT(""),1);

	if(tSIS==NULL)
	{
    	SetStatusText("tSIS nicht vorhanden");
    	return 0;
	}else{
		SetStatusText("tSIS vorhanden");
		try
		{
    		//SetStatusText(_("Einstellungen für Stereogramm lesen"));
    		if(ParameterAnpassen())
    		{
                SetStatusText(wxT("Erzeugung beginnt"));
    			try
    			{
    				SIStereogramm = tSIS->HoleStereogramm();
    			}
    			catch(Error* e)
    			{
        			SetStatusText(wxT("Erzeugung abgebrochen"));
        			SetStatusText(e->message(), 1);
        			return 0;
    			}
            }else{
				SetStatusText(wxT("Erzeugung abgebrochen"));
				SetStatusText(_("Einstellungen fehlerhaft"),1);
				*SIStereogramm = *SISHoehenkarte;
                return 0;
			}
		}
		catch(Error* e)
		{
			//SetStatusText("Fehler! (");
			//SetStatusText(e->message(), 1);
			return 0;
		}
	}
    SetStatusText(wxT("Erzeugung erledigt"));
    SetStatusText(wxT("Stereogramm angezeigt"),1);
	return 1;
}

void SISFrame::OnMouseWheel(wxMouseEvent &event)
{
	if(zoomen)
	{
        if(zoomenTextur)
    	{
    		if(event.GetWheelRotation()>0)
    		{
                TexturZoomAnpassen(TexturZoom, 0);
    		}else{
                TexturZoomAnpassen(TexturZoom, 1);
    		}
    		SetStatusText(_("TexturZoom = ") + wxString::Format(wxT("%.2f"),TexturZoom));
    	}else{
    		if(event.GetWheelRotation()>0)
            {
                ZoomAnpassen(ZoomFkt, 0);
            }else{
    			ZoomAnpassen(ZoomFkt, 1);
    		}
    		SetStatusText(_("Zoom = ") + wxString::Format(wxT("%.2f"),ZoomFkt));
        }
	}
	event.Skip();
	return;
}


wxImage SISFrame::LeinwandBemalen(SIS_Bild* tempBild)
{
	wxImage tempLeinwand(tempBild->Breite(),tempBild->Hoehe());

	int Groesse = tempBild->Breite()*tempBild->Hoehe()*3;
	unsigned char* DatenPuffer = (unsigned char*) malloc(Groesse);
	for (int i=0; i<Groesse; i++)
	{
		try
		{
			DatenPuffer[i] = tempBild->Daten(i);
		}
		catch(Error* e)
		{
			SetStatusText(_(e->message()),1);
			//delete e;
			return wxImage(10,10);
		}
		catch(...)
		{
			SetStatusText(wxT("Unbekannter Fehler"),1);
			return wxImage(10,10);
		}
	}
	if(tempLeinwand.Ok())
	{
//		wxMessageDialog(this, _("Leinwand wird bemalt.")).ShowModal();
		tempLeinwand.SetData(DatenPuffer);
	}else{
		free (DatenPuffer);
		SetStatusText(wxT("Leinwand ist nicht richtig bemalt"),1);
	}
	return tempLeinwand;
}

void SISFrame::Ueberblenden(void)
{
	Leinwand = LeinwandBemalen(SISHoehenkarte);
	if(Leinwand.Ok())
	{
		int offsetBreite = int(TexRelLage.x());
		int offsetHoehe = int(TexRelLage.y());
		int HKBreite = Leinwand.GetWidth();
		int HKHoehe = Leinwand.GetHeight();
		int TexturBreite = SISTextur->Breite();
		int TexturHoehe = SISTextur->Hoehe();
		int maxBreite = (HKBreite<(offsetBreite+TexturBreite)) ? HKBreite : (offsetBreite+TexturBreite);
		int maxHoehe = (HKHoehe<(offsetHoehe+TexturHoehe)) ? HKHoehe : (offsetHoehe+TexturHoehe);
		int minX = (offsetBreite>0) ? offsetBreite : 0;
		int minY = (offsetHoehe>0) ? offsetHoehe : 0;

		unsigned char *Ziel, *Textur;

		Ziel = Leinwand.GetData();
		Textur = SISTextur->Datensatz();

		for(int Reihe = minY; Reihe < maxHoehe; Reihe++)
		{
			for(int Spalte = minX; Spalte < maxBreite; Spalte++)
			{
				Ziel[(Reihe*HKBreite+Spalte)*3] = (Textur[((Reihe-offsetHoehe)*TexturBreite + (Spalte-offsetBreite))*3]
													+ Ziel[(Reihe*HKBreite+Spalte)*3]) / 2;
				Ziel[(Reihe*HKBreite+Spalte)*3 + 1] = (Textur[((Reihe-offsetHoehe)*TexturBreite + (Spalte-offsetBreite))*3 + 1]
													+ Ziel[(Reihe*HKBreite+Spalte)*3 + 1]) / 2;
				Ziel[(Reihe*HKBreite+Spalte)*3 + 2] = (Textur[((Reihe-offsetHoehe)*TexturBreite + (Spalte-offsetBreite))*3 + 2]
													+ Ziel[(Reihe*HKBreite+Spalte)*3 + 2]) / 2;
			}
		}
		return;
	}else{
		return;
	}
}

float SISFrame::AnzeigeSkalieren(wxDC& dc)
{
    return 1.0;
}

bool SISFrame::LagePruefen(void)
{
    bool veraendert = 0;
	if (TexAbsLage.x() <= (0-float(imgTextur.GetWidth()+1)))
    {
        TexAbsLage.SetKoordinaten(0, (0-float(imgTextur.GetWidth()+1)));
        veraendert = 1;
    }
	if (TexAbsLage.y() <= (0-float(imgTextur.GetHeight()+1)))
    {
        TexAbsLage.SetKoordinaten(1, (0-float(imgTextur.GetHeight()+1)));
        veraendert = 1;
    }

	if (TexAbsLage.x() > (imgHoehenkarte.GetWidth()-1))
    {
        TexAbsLage.SetKoordinaten(0, float(imgHoehenkarte.GetWidth()-1));
        veraendert = 1;
    }
	if (TexAbsLage.y() > (imgHoehenkarte.GetHeight()-1))
    {
        TexAbsLage.SetKoordinaten(1, float(imgHoehenkarte.GetHeight()-1));
        veraendert = 1;
    }

	return veraendert;
}

void SISFrame::OnKeyPress(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_CONTROL)
    {
        keineNeuBerechnung = 1;
        zoomen = 1;
        alterZoom = ZoomFkt;
    }
	if(event.GetKeyCode() == WXK_SHIFT) Beschleuniger = 10.0;

	//Bewegen
	int deltaX = 0;
	int deltaY = 0;
	if(event.GetKeyCode() == WXK_LEFT) deltaX -= (int)(1*Beschleuniger);
    if(event.GetKeyCode() == WXK_UP) deltaY -= (int)(1*Beschleuniger);
    if(event.GetKeyCode() == WXK_RIGHT) deltaX += (int)(1*Beschleuniger);
    if(event.GetKeyCode() == WXK_DOWN) deltaY += (int)(1*Beschleuniger);
    if((deltaX!=0)||(deltaY!=0))
    {
        Verschieben(deltaX, deltaY);
    }

	if(zoomen)
	{
    	if(zoomenTextur)
    	{
            if(event.GetKeyCode() == '-')TexturZoomAnpassen(TexturZoom, 0);
    		if(event.GetKeyCode() == '+')TexturZoomAnpassen(TexturZoom, 1);
    		SetStatusText(_("TexturZoom = ") + wxString::Format(wxT("%.2f"),TexturZoom));
    	}else{
    		if(event.GetKeyCode() == '-')ZoomAnpassen(ZoomFkt, 0);
    		if(event.GetKeyCode() == '+')ZoomAnpassen(ZoomFkt, 1);
    		SetStatusText(_("Zoom = ") + wxString::Format(wxT("%.2f"),ZoomFkt));
        }
    }
	if(event.GetKeyCode() == WXK_F12)
    {
        if(zoomen)
        {
    		if(zoomenTextur)
    		{
                TexturZoom = 1.0;
                TexturAufbringen();
    		    SetStatusText(_("TexturZoom = ") + wxString::Format(wxT("%.2f"),TexturZoom));
            }else{
                ZoomFkt = 1.0;
                TexturAufbringen();
                HoehenKarteAufbringen();
            	SetStatusText(_("Zoom = ") + wxString::Format(wxT("%.2f"),ZoomFkt));
            }
        }else{
            TexAbsLage = Vektor(0.0, 0.0, 0.0);
            TexRelLage = TexAbsLage * ZoomFkt;
        }
        geaendert=1;
        Refresh();
    }
	if(event.GetKeyCode() == WXK_F1)
    {
        AnleitungOeffnen();
    }
	event.Skip();
	return;
}

void SISFrame::OnKeyRelease(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_CONTROL)
    {
        keineNeuBerechnung = 0;
        zoomen = 0;
        Refresh();
    }
	if(event.GetKeyCode() == WXK_SHIFT) Beschleuniger = 1.0;
	event.Skip();
	return;
}

void SISFrame::OnLeftDown(wxMouseEvent& event)
{
    wxClientDC dc(this);
	AlteMousePosition = event.GetLogicalPosition(dc);
	if(StartVerschieben)
    {
        StartPunkt = (int)((AlteMousePosition.x-BILDEINRUECKUNG));/*/ZoomFkt);*/
        if(StartPunkt<0)StartPunkt=0;
        if(StartPunkt>= (int)SISHoehenkarte->Breite())
                StartPunkt = (int)(SISHoehenkarte->Breite()-1);
        Refresh();
    }
    event.Skip();
    return;
}

void SISFrame::OnMouseMove(wxMouseEvent& event)
{
    if(event.Dragging() && event.LeftIsDown())
    {
        wxClientDC dc(this);
        NeueMousePosition = event.GetLogicalPosition(dc);
        wxPoint Differenz = wxPoint((int)((NeueMousePosition.x-AlteMousePosition.x)/ZoomFkt),
                                    (int)((NeueMousePosition.y-AlteMousePosition.y)/ZoomFkt));

        Verschieben((int)(Differenz.x), (int) (Differenz.y));

        AlteMousePosition = NeueMousePosition;
    }
    event.Skip();
    return;
}

void SISFrame::Verschieben(int x, int y)
{
    if(StartVerschieben)
    {
        StartPunkt += x;
        if(StartPunkt<0)StartPunkt=0;
        if(StartPunkt>= imgHoehenkarte.GetWidth()) StartPunkt = (int)(imgHoehenkarte.GetWidth()-1);
        tSIS->SetzeStartPunkt((int)(StartPunkt));//*ZoomFkt));
    }else{
        Vektor tempVektor = Vektor((float)(x), (float)(y), 0);
        TexAbsLage += tempVektor;
        TexRelLage = TexAbsLage * ZoomFkt;
        SetStatusText(wxString::Format(wxT("%.2f"), TexAbsLage.x()) + wxT(" ") + wxString::Format(wxT("%.2f"), TexAbsLage.y()),1);
        if(LagePruefen())
        {
            TexRelLage = TexAbsLage * ZoomFkt;
        }
    }
    geaendert=1;
    Refresh();
    return;
}

void SISFrame::OnLeftUp(wxMouseEvent& event)
{
	SetStatusText(wxT(""),1);
	if(StartVerschieben)
    {
        tSIS->SetzeStartPunkt((int)(StartPunkt));//*ZoomFkt));
        geaendert=1;
    }
	Refresh();
    event.Skip();
    return;
}

void SISFrame::OnEraseBackground(wxEraseEvent& event)
{
    //event.Skip();
    return;
}

bool SISFrame::ParameterAnpassen(void)
{
    double *tempDbl = new double;

    if(TXT_AugenAbstand->GetValue().ToDouble(tempDbl))
    {
        tSIS->SetzeAugenabstand((int)((*tempDbl)*DotsPerInch*ZoomFkt/2.54));
    }else{
        wxMessageDialog(this, _("Fehler bei Augenabstand")).ShowModal();
        return 0;
    }

    if(TXT_MattscheibenAbstand->GetValue().ToDouble(tempDbl))
    {
        tSIS->SetzeMattscheibenAbstand((int)((*tempDbl)*DotsPerInch*ZoomFkt/2.54));
    }else{
        wxMessageDialog(this, _("Fehler bei Mattscheibenabstand")).ShowModal();
        return 0;
    }

    if(TXT_Verschub->GetValue().ToDouble(tempDbl))
    {
        tSIS->SetzeVerschub((int)((*tempDbl)*DotsPerInch*ZoomFkt/2.54));
    }else{
        wxMessageDialog(this, _("Fehler bei Verschub")).ShowModal();
        return 0;
    }

    if(TXT_BildTiefe->GetValue().ToDouble(tempDbl))
    {
        tSIS->SetzeBildtiefe((int)((*tempDbl)*DotsPerInch*ZoomFkt/2.54));
    }else{
        wxMessageDialog(this, _("Fehler bei Bildtiefe")).ShowModal();
        return 0;
    }

	TXT_BildAbstand->SetValue(wxString::Format(wxT("%.2f"), (tSIS->HoleBildabstand()*2.54f/(DotsPerInch*ZoomFkt) )));

	delete tempDbl;

	tSIS->SetzeTexturBreitenNullage(int(TexRelLage.x()));
	tSIS->SetzeTexturHoehenNullage(int(TexRelLage.y()));

	return 1;
}

void SISFrame::TexturAufbringen(void)
{
	if(imgTextur.Ok())
	{
         /*Zoomen*/
        wxImage TempImg = imgTextur.Scale((int)(imgTextur.GetWidth()*ZoomFkt*TexturZoom), (int)(imgTextur.GetHeight()*ZoomFkt*TexturZoom));
		unsigned char * Puffer = TempImg.GetData();
		try
		{
			SISTextur->Breite(TempImg.GetWidth());
			SISTextur->Hoehe(TempImg.GetHeight());
		}
		catch(Error*e)
		{
			SetStatusText(_(e->message()),1);
			//delete e;
			return;
		}
		for(unsigned int i = 0; i < (SISTextur->Breite()*SISTextur->Hoehe()*3);i++)
		{
			try
			{
				SISTextur->Daten(i,Puffer[i]);
			}
			catch(Error*e)
			{
				SetStatusText(_(e->message()),1);
				//delete e;
				return;
			}
		}
    	//Refresh();
    	return;
	}
	SetStatusText(_("Fehler beim Texturieren"));
    return;
}

void SISFrame::HoehenKarteAufbringen()
{
	if(imgHoehenkarte.Ok())
	{
        /*Zoomen*/
        wxImage TempImg = imgHoehenkarte.Scale((int)(imgHoehenkarte.GetWidth()*ZoomFkt), (int)(imgHoehenkarte.GetHeight()*ZoomFkt));
		unsigned char * Puffer = TempImg.GetData();
		try
		{
			SISHoehenkarte->Breite(TempImg.GetWidth());
			SISHoehenkarte->Hoehe(TempImg.GetHeight());
		}
		catch(Error*e)
		{
			SetStatusText(_(e->message()),1);
			//delete e;
			return;
		}
		for(unsigned int i = 0; i < (SISHoehenkarte->Breite()*SISHoehenkarte->Hoehe()*3);i++)
		{
			try
			{
				SISHoehenkarte->Daten(i,Puffer[i]);
			}
			catch(Error*e)
			{
				SetStatusText(_(e->message()),1);
				//delete e;
				return;
			}
		}
		SetStatusText(_("Höhenkarte aufgebracht"));
    	return;
	}
	SetStatusText(_("Fehler beim Öffnen"));
	return;
}

void SISFrame::RDTexturAnpassen(void)
{
	if(SIS_Textur_Mode == SIS_RND_RGB_TEXTUR||SIS_Textur_Mode == SIS_RND_BW_TEXTUR)
	{
		try
		{
			SISTextur->Hoehe(SISHoehenkarte->Hoehe());
			SISTextur->Breite(SISHoehenkarte->Breite());
			SISTextur->ZufallsWerte(SIS_Textur_Mode);//0 für RGB - 1 für BW
		}
		catch(Error* e)
		{
			wxMessageDialog(this, _(e->message()), _("Fehler")).ShowModal();
			//delete e;
		}
	}
	if(LagePruefen())
    {
        TexRelLage = TexAbsLage * ZoomFkt;
    }
	return;
}

void SISFrame::OnEinstellungenGeaendert(wxCommandEvent &event)
{
	int WertID = event.GetId();
	double *tempDbl = new double;

    if(WertID == ID_SIS_DOTSPERINCH)
    {
        if(TXT_DotsPerInch->GetValue().ToDouble(tempDbl))
        {
            DotsPerInch = (int)(*tempDbl);
        }else{
            wxMessageDialog(this, _("Fehler bei dpi")).ShowModal();
            return;
        }
    }else{
        if(!ParameterAnpassen())
        {
            SetStatusText("Einstellungen fehlerhaft");
        }
    }

	delete tempDbl;
	geaendert=1;
	Refresh();
    event.Skip();
    return;
}

void SISFrame::EingabefelderFuellen(void)
{
	TXT_AugenAbstand->SetValue(wxString::Format(wxT("%.2f"), tSIS->HoleAugenabstand()*2.54f/(DotsPerInch*ZoomFkt)));
	TXT_DotsPerInch->SetValue(wxString::Format(wxT("%i"), DotsPerInch));
	TXT_MattscheibenAbstand->SetValue(wxString::Format(wxT("%.2f"), tSIS->HoleMattscheibenAbstand()*2.54f/(DotsPerInch*ZoomFkt)));
	TXT_Verschub->SetValue( wxString::Format( wxT("%.2f"), tSIS->HoleVerschub() *2.54f/(DotsPerInch*ZoomFkt)));
	TXT_BildTiefe->SetValue(wxString::Format(wxT("%.2f"), (tSIS->HoleBildtiefe()*2.54f/(DotsPerInch*ZoomFkt) )));
	TXT_BildAbstand->SetValue(wxString::Format(wxT("%.2f"), (tSIS->HoleBildabstand()*2.54f/(DotsPerInch*ZoomFkt) )));
	return;
}


void SISFrame::OnStartVerschieben(wxCommandEvent &event)
{
     if(event.IsChecked())
    {
		StartVerschieben = true;
    }else{
		StartVerschieben = false;
	}
    Refresh();
    return;
}

void SISFrame::ZoomAnpassen(float& zoom, bool plus)
{
    if(!plus)
	{
		zoom -= 0.01 * Beschleuniger;
	}
    if(plus)
    {
		zoom += 0.01 * Beschleuniger;
	}
	if(zoom < 0.01)zoom = 0.01;
	if(zoom > 3.0)zoom = 3.0;
	TexRelLage = TexAbsLage * zoom;

	HoehenKarteAufbringen();
	TexturAufbringen();
	geaendert=1;
	Refresh();
	return;
}

void SISFrame::TexturZoomAnpassen(float& zoom, bool plus)
{
    if(!plus)
	{
		zoom -= 0.01 * Beschleuniger;
	}
    if(plus)
    {
		zoom += 0.01 * Beschleuniger;
	}
	if(zoom < 0.01)zoom = 0.01;
	if(zoom > 10.0)zoom = 10.0;
	TexturAufbringen();
	geaendert=1;
	Refresh();
	return;
}


void SISFrame::OnAnleitung(wxCommandEvent &event)
{
    AnleitungOeffnen();
    return;
}

void SISFrame::OnBelegung(wxCommandEvent &event)
{
    TextAnzeige* Feld = new TextAnzeige(_("Doc/Belegung.tst"), _("Zeigt die Belegung der Tastatur und Maustasten an"),this, _("Tastaturbelegung"), wxPoint(400,50), wxSize(800,600) ,wxDEFAULT_FRAME_STYLE);
    Feld->Show();
    return;
}

void SISFrame::OnLizenz(wxCommandEvent &event)
{
    TextAnzeige* Feld = new TextAnzeige(_("Doc/GNUGPL.liz"), _("GNU GENERAL PUBLIC LICENSE"),this, _("Lizenz"), wxPoint(400,50), wxSize(800,600) ,wxDEFAULT_FRAME_STYLE);
    Feld->Show();
    return;
}

void SISFrame::AnleitungOeffnen(void)
{
    TextAnzeige* Feld = new TextAnzeige(_("Doc/Hilfe.hlf"), _("Übersicht über die Funktionen des Programms"),this, _("Anleitung"), wxPoint(400,50), wxSize(800,600) ,wxDEFAULT_FRAME_STYLE);
    Feld->Show();
    return;
}

void SISFrame::OnARDS(wxCommandEvent &event)
{
    TextAnzeige* Feld = new TextAnzeige(_("Doc/Ards.hlf"), _("Ein kurzes Pamphlet"),this, _("Rechengang der SIS"), wxPoint(400,50), wxSize(800,600) ,wxDEFAULT_FRAME_STYLE);
    Feld->Show();
    return;
}
