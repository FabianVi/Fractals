#include <math.h>
#include <iostream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/gdicmn.h>

#include "Config.h.in";

#include "Algorithm.h";

class BasicDrawPane : public wxPanel
{
private:

public:
    BasicDrawPane(wxFrame* parent);
    void paintEvent(wxPaintEvent & evt);

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
    EVT_PAINT(BasicDrawPane::paintEvent)
END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent){
}

/**
 * will be called if it needs to be repainted or Update() or Refresh() gets called on th Class N.B. handels repaints
 * @param evt
 */
void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    wxClientDC dc(this);

    int w;
    int h;
    this->GetSize(&w,&h);

    std::cout << "W: " << w << " H: " << h << std::endl;

    dc.Clear();

    unsigned char* imdata = (unsigned char*) malloc( w * h * 3 );

    Algorithm::Mandelbrot(imdata,w,h);

    wxImage m_image = wxImage(w, h, imdata);
    wxBitmap m_bitmap = wxBitmap(m_image);

    dc.DrawBitmap(m_bitmap,0,0,true);
}


class MyFrame : public wxFrame
{
public:
    MyFrame();
private:
    BasicDrawPane * drawPane;
};

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Fractals",wxPoint(50,50), wxSize(1200,800)) {
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    drawPane = new BasicDrawPane( (wxFrame*) this);

    sizer->Add(drawPane, 1, wxEXPAND);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);
};



class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

