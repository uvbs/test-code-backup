/////////////////////////////////////////////////////////////////////////////
// Name:        virtual.cpp
// Purpose:     wxHtml sample: demonstrates virtual file systems feature
// Author:      ?
// Modified by:
// Created:     ?
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/html/htmlwin.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../../sample.xpm"
#endif

// new handler class:

#include "wx/wfstream.h"
#include "wx/mstream.h"


// ----------------------------------------------------------------------------
// MyVFS
// ----------------------------------------------------------------------------

class MyVFS : public wxFileSystemHandler
{
public:
    MyVFS() : wxFileSystemHandler() {}

    wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
    bool CanOpen(const wxString& location);
};

bool MyVFS::CanOpen(const wxString& location)
{
    return (GetProtocol(location) == wxT("myVFS"));
}

wxFSFile* MyVFS::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxFSFile *f;
    wxInputStream *str;
    static char buf[1024];
    const wxWX2MBbuf loc = location.ToAscii();

    sprintf(buf, "<html><body><h2><i>You're in Node <u>%s</u></i></h2><p>"
                "Where do you want to go?<br><blockquote>"
                "<a href=\"%s-1\">sub-1</a><br>"
                "<a href=\"%s-2\">sub-2</a><br>"
                "<a href=\"%s-3\">sub-3</a><br>"
                "</blockquote></body></html>",
                (const char*)loc, (const char*)loc, (const char*)loc,
                (const char*)loc);

    // NB: There's a terrible hack involved: we fill 'buf' with new data every
    //     time this method is called and return new wxMemoryInputStream pointing to it.
    //     This won't work as soon as there are 2+ myVFS files opened. Fortunately,
    //     this won't happen because wxHTML keeps only one "page" file opened at the
    //     time.
    str = new wxMemoryInputStream(buf, strlen(buf));
    f = new wxFSFile(str, location, wxT("text/html"), wxEmptyString, wxDateTime::Today());

    return f;
}



// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnBack(wxCommandEvent& event);
    void OnForward(wxCommandEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
        // menu items
    Minimal_Quit = 1,
    Minimal_Back,
    Minimal_Forward,

        // controls start here (the numbers are, of course, arbitrary)
    Minimal_Text = 1000
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_Back, MyFrame::OnBack)
    EVT_MENU(Minimal_Forward, MyFrame::OnForward)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    // Create the main application window
    MyFrame *frame = new MyFrame(_("wxHtmlWindow testing application"),
        wxDefaultPosition, wxSize(640, 480));

    // Show it
    frame->Show(true);

    wxFileSystem::AddHandler(new MyVFS);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

wxHtmlWindow *html;

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    SetIcon(wxICON(sample));

    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuNav = new wxMenu;

    menuFile->Append(Minimal_Quit, _("E&xit"));
    menuNav->Append(Minimal_Back, _("Go &BACK"));
    menuNav->Append(Minimal_Forward, _("Go &FORWARD"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuNav, _("&Navigate"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    html = new wxHtmlWindow(this);
    html -> SetRelatedFrame(this, _("VFS Demo: '%s'"));
#if wxUSE_STATUSBAR
    html -> SetRelatedStatusBar(1);
#endif // wxUSE_STATUSBAR
    html -> LoadPage(wxT("start.htm"));
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnBack(wxCommandEvent& WXUNUSED(event))
{
    if (!html -> HistoryBack()) wxMessageBox(_("You reached prehistory era!"));
}

void MyFrame::OnForward(wxCommandEvent& WXUNUSED(event))
{
    if (!html -> HistoryForward()) wxMessageBox(_("No more items in history!"));
}
