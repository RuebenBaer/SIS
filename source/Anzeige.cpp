#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/dcbuffer.h>
#include "Anzeige.h"

BEGIN_EVENT_TABLE(AnzeigeFrame, wxDialog)
   EVT_SIZE(AnzeigeFrame::OnSize)
   EVT_PAINT(AnzeigeFrame::OnPaint)
   EVT_MOUSEWHEEL(AnzeigeFrame::OnMouseWheel)
   EVT_KEY_DOWN(AnzeigeFrame::OnKeyPress)
   EVT_KEY_UP(AnzeigeFrame::OnKeyRelease)
   EVT_ERASE_BACKGROUND(AnzeigeFrame::OnEraseBackground)
   EVT_SET_FOCUS(AnzeigeFrame::OnGotFocus)
END_EVENT_TABLE()

AnzeigeFrame::AnzeigeFrame(wxWindow* parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxDialog(parent, id, title, pos, size, wxCAPTION|wxRESIZE_BORDER, wxT("Anzeige"))
{
	Leinwand = wxImage(100,100);
	ZoomFkt = 1.0;
	Beschleuniger = 1;
    Refresh(false);
}

AnzeigeFrame::~AnzeigeFrame()
{
    //delete Leinwand;
}

void AnzeigeFrame::OnEraseBackground(wxEraseEvent &event)
{
	return;
}

void AnzeigeFrame::OnSize(wxSizeEvent &event)
{
	Refresh(false);
	return;
}

void AnzeigeFrame::OnPaint(wxPaintEvent &event)
{
	wxBufferedPaintDC dc(this);

	/*Hintergrund zeichnen*/
    wxRect rect(wxPoint(0, 0), GetClientSize());
    wxColour back = GetBackgroundColour();
    dc.SetBrush(wxBrush(back));
    dc.SetPen(wxPen(back, 1));
    dc.DrawRectangle(rect);

    if(Leinwand.Ok())
	{
		int tempBreite = int(Leinwand.GetWidth() * ZoomFkt);
		int tempHoehe = int(Leinwand.GetHeight() * ZoomFkt);
		dc.DrawBitmap(wxBitmap(Leinwand.Scale(tempBreite, tempHoehe)), wxPoint(0, 0), true);
	}
}

void AnzeigeFrame::OnMouseWheel(wxMouseEvent &event)
{
	if (zoomen)
	{
		if(event.GetWheelRotation()<0)
		{
			ZoomFkt -= 0.01 * Beschleuniger;
		}else{
			ZoomFkt += 0.01 * Beschleuniger;
		}
		if(ZoomFkt < 0.001)ZoomFkt = 0.001;
		if(ZoomFkt > 5.0)ZoomFkt = 5.0;
		Refresh();
	}
	event.Skip();
	return;
}

void AnzeigeFrame::OnKeyPress(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_CONTROL) zoomen = 1;
	if(event.GetKeyCode() == WXK_SHIFT) Beschleuniger = 10.0;
	if (zoomen)
	{
		if(event.GetKeyCode() == '-')
		{
			ZoomFkt -= 0.01 * Beschleuniger;
		}
        if(event.GetKeyCode() == '+')
        {
			ZoomFkt += 0.01 * Beschleuniger;
		}
		if(ZoomFkt < 0.01)ZoomFkt = 0.01;
		if(ZoomFkt > 5.0)ZoomFkt = 5.0;
		Refresh();
	}
	if(event.GetKeyCode() == WXK_F12)
    {
        ZoomFkt = 1.0;
        Refresh();
    }
	if(event.GetKeyCode() == WXK_ESCAPE)
    {
        wxCloseEvent ev = wxCloseEvent(0, 7777);
        GetEventHandler()->ProcessEvent(ev);
        Hide();
    }
	event.Skip();
	return;
}

void AnzeigeFrame::OnKeyRelease(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_CONTROL) zoomen = 0;
	if(event.GetKeyCode() == WXK_SHIFT) Beschleuniger = 1.0;
	event.Skip();
	return;
}

bool AnzeigeFrame::SetAnzeigeBild(SIS_Bild *tempBild)
{
	try
	{
    	if(tempBild!=NULL)
    	{
    		int Groesse = tempBild->Hoehe()*tempBild->Breite()*3;
    		unsigned char *von, *nach;
    		Leinwand.Resize(wxSize(tempBild->Breite(), tempBild->Hoehe()), wxPoint(0, 0));
    		if(Leinwand.Ok())
    		{
    			von = tempBild->Datensatz();
    			nach = Leinwand.GetData();
    			for(int i=0; i<Groesse; i++)
    			{
    				try
    				{
    					nach[i]=von[i];
    				}
    				catch(...)
    				{
    					wxMessageDialog(this, wxString::FromAscii("Fehler bei Datenkopie - i = ") + wxString::Format(wxT("%i"), i)).ShowModal();
    					return(0);
    				}
    			}
    			return(1);
    		}else{
    			wxMessageDialog(this, wxString::FromAscii("Leinwand nicht OK")
    								+ wxT("\nBreite: ") + wxString::Format(wxT("%i"), tempBild->Breite())
    								+ wxString::FromAscii("\nHöhe: ") + wxString::Format(wxT("%i"), tempBild->Hoehe())).ShowModal();
    			return(0);
    		}
    	}else{
    		wxMessageDialog(this, wxString::FromAscii("tempBild nicht OK")).ShowModal();
    		return(0);
    	}
	}
	catch(...)
	{
		wxMessageDialog(this, wxString::FromAscii("Ominöser Fehler")).ShowModal();
		return(0);
	}
	return(0);
}

wxImage AnzeigeFrame::GetAnzeigeBild(void)
{
    return Leinwand;
}

void AnzeigeFrame::OnGotFocus(wxFocusEvent &event)
{
	return;
}

void AnzeigeFrame::NeuZeichnen(void)
{
	Refresh();
	return;
}
