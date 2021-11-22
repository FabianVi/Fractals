#include <math.h>
#include <iostream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/gdicmn.h>

#include "Config.h.in"

#include "Vector.h"
#include "Algorithm.h"

#include "benchmark.h"

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

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent){}

/**
 * will be called if it needs to be repainted or Update() or Refresh() gets called on th Class N.B. handels repaints
 * @param evt
 */
void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    ScopedTimer sc("paintEvent");
    DestinctTimer dt1("draw img");

    int w;
    int h;
    this->GetSize(&w,&h);

    unsigned char* imdata = (unsigned char*) malloc( w * h * 3 );
    Algorithm::Mandelbrot(imdata,Vector2D<int>(w , h),1000);

    dt1.start();
    wxClientDC dc(this);

    dc.Clear();
    wxImage m_image = wxImage(w, h, imdata, true);
    wxBitmap m_bitmap = wxBitmap(m_image);

    dc.DrawBitmap(m_bitmap,0,0,true);
    dt1.stop();
}


class MyFrame : public wxFrame
{
public:
    MyFrame();
private:
    BasicDrawPane * drawPane;
};

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Fractals",wxPoint(50,50), wxSize(800,800)) {
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

