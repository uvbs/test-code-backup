/////////////////////////////////////////////////////////////////////////////
// Name:        joytest.cpp
// Purpose:     Joystick sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_JOYSTICK
#   error You must set wxUSE_JOYSTICK to 1 in setup.h
#endif

#include "wx/sound.h"
#include "wx/joystick.h"

#include "joytest.h"

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

MyFrame *frame = NULL;

IMPLEMENT_APP(MyApp)

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

int winNumber = 1;

int nButtons = 0;
// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxJoystick stick(wxJOYSTICK1);
    if (!stick.IsOk())
    {
        wxMessageBox(wxT("No joystick detected!"));
        return false;
    }

#if wxUSE_SOUND
    m_fire.Create(wxT("buttonpress.wav"));
#endif // wxUSE_SOUND

    m_minX = stick.GetXMin();
    m_minY = stick.GetYMin();
    m_maxX = stick.GetXMax();
    m_maxY = stick.GetYMax();

    // Create the main frame window

    frame = new MyFrame(NULL, wxT("Joystick Demo"), wxDefaultPosition,
        wxSize(500, 400), wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);

    frame->SetIcon(wxICON(sample));

    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(JOYTEST_QUIT, wxT("&Exit"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, wxT("&File"));

    // Associate the menu bar with the frame
    frame->SetMenuBar(menu_bar);

#if wxUSE_STATUSBAR
    frame->CreateStatusBar();
    frame->SetStatusText(wxString::Format(wxT("Device [%s] (PID:[%i] MID:[%i]) Ready... # of joysticks:[%i]"), stick.GetProductName().c_str(), stick.GetProductId(), stick.GetManufacturerId(), wxJoystick::GetNumberJoysticks()));
#endif // wxUSE_STATUSBAR

    frame->CenterOnScreen();
    frame->Show(true);

    return true;
}

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_JOYSTICK_EVENTS(MyCanvas::OnJoystickEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size):
    wxScrolledWindow(parent, wxID_ANY, pos, size, wxSUNKEN_BORDER)
{
    m_stick = new wxJoystick(wxJOYSTICK1);
    nButtons = m_stick->GetNumberButtons();
    m_stick->SetCapture(this, 10);
}

MyCanvas::~MyCanvas()
{
    m_stick->ReleaseCapture();
    delete m_stick;
}

void MyCanvas::OnJoystickEvent(wxJoystickEvent& event)
{
    // We don't have valid (x, y) coordinates for z-move events.
    if ( !event.IsZMove() )
    {
        wxClientDC dc(this);

        wxPoint pt(event.GetPosition());

        // if negative positions are possible then shift everything up
        int xmin = wxGetApp().m_minX;
        int xmax = wxGetApp().m_maxX;
        int ymin = wxGetApp().m_minY;
        int ymax = wxGetApp().m_maxY;
        if (xmin < 0) {
            xmax += abs(xmin);
            pt.x += abs(xmin);
        }
        if (ymin < 0) {
            ymax += abs(ymin);
            pt.y += abs(ymin);
        }

        // Scale to canvas size
        int cw, ch;
        GetSize(&cw, &ch);

        pt.x = (long) (((double)pt.x/(double)xmax) * cw);
        pt.y = (long) (((double)pt.y/(double)ymax) * ch);

        if (xpos > -1 && ypos > -1 && event.IsMove() && event.ButtonIsDown())
        {
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawLine(xpos, ypos, pt.x, pt.y);
        }

        xpos = pt.x;
        ypos = pt.y;
    }

#if wxUSE_STATUSBAR
    wxString buf;
    if (event.ButtonDown())
        buf.Printf(wxT("Joystick (%d, %d) #%i Fire!"), xpos, ypos, event.GetButtonChange());
    else
        buf.Printf(wxT("Joystick (%d, %d)  "), xpos, ypos);

/*
    for(int i = 0; i < nButtons; ++i)
    {
        buf += wxString(wxT("[")) +
        ((event.GetButtonState() & (1 << i)) ? wxT("Y") : wxT("N")) + wxString(wxT("]"));
    }
*/

    frame->SetStatusText(buf);
#endif // wxUSE_STATUSBAR

#if wxUSE_SOUND
    if (event.ButtonDown() && wxGetApp().m_fire.IsOk())
    {
        wxGetApp().m_fire.Play();
    }
#endif // wxUSE_SOUND
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(JOYTEST_QUIT, MyFrame::OnQuit)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *parent, const wxString& title, const wxPoint& pos,
    const wxSize& size, const long style)
    : wxFrame(parent, wxID_ANY, title, pos, size, style)
{
    canvas = new MyCanvas(this);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive() && canvas)
        canvas->SetFocus();
}
