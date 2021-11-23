#include <cmath>
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
    Vector2x2<double> view;
public:
    explicit BasicDrawPane(wxFrame* parent);
    void paintEvent(wxPaintEvent & evt);
    void OnClick(wxMouseEvent& evt);
    void Mousewheel(wxMouseEvent& evt);

    void HandleKey(wxKeyEvent& evt);

    void zoom(double factor);
};

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent), view(-2.0,-1.0,1.0,1.0){
    Bind(wxEVT_PAINT,&BasicDrawPane::paintEvent,this);
    Bind(wxEVT_LEFT_UP,&BasicDrawPane::OnClick,this);
    Bind(wxEVT_MOUSEWHEEL,&BasicDrawPane::Mousewheel,this);
    Bind(wxEVT_KEY_DOWN,&BasicDrawPane::HandleKey,this);
}

/**
 * will be called if it needs to be repainted or Update() or Refresh() gets called on th Class N.B. handles repaints
 * @param evt
 */
void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    ScopedTimer sc("paintEvent");
    DestinctTimer dt1("draw img");

    int w;
    int h;
    this->GetSize(&w,&h);

    auto* im_data = (unsigned char*) malloc(w * h * 3 );
    Algorithm::Mandelbrot(im_data, Vector2D<int>(w , h), 100, view);

    dt1.start();
    wxClientDC dc(this);

    dc.Clear();
    wxImage m_image = wxImage(w, h, im_data, true);
    wxBitmap m_bitmap = wxBitmap(m_image);

    dc.DrawBitmap(m_bitmap,0,0,true);
    dt1.stop();
}

void BasicDrawPane::OnClick(wxMouseEvent& evt){
    int x, y;
    evt.GetPosition(&x,&y);

    int w,h;
    this->GetSize(&w,&h);

    double dx =  (view.x2-view.x1)*(((double)x)/w -0.5);
    double dy =  (view.y2-view.y1)*(((double)y)/h -0.5);

    view.x1 += dx;
    view.x2 += dx;
    view.y1 += dy;
    view.y2 += dy;

    this->zoom(0.1);

    BasicDrawPane::Refresh();
}

void BasicDrawPane::Mousewheel(wxMouseEvent &evt) {
    int rot = evt.GetWheelRotation();
    double zoomFactor = rot * 0.001;

    int x, y;
    evt.GetPosition(&x,&y);

    int w,h;
    this->GetSize(&w,&h);

    double dx =  (view.x2-view.x1)*(((double)x)/w -0.5);
    double dy =  (view.y2-view.y1)*(((double)y)/h -0.5);

    view.x1 += dx * zoomFactor * 5;
    view.x2 += dx * zoomFactor * 5;
    view.y1 += dy * zoomFactor * 5;
    view.y2 += dy * zoomFactor * 5;

    zoom(zoomFactor);
    BasicDrawPane::Refresh();
}

void BasicDrawPane::zoom(double factor) {
    view.x1 += factor*(view.x2-view.x1);
    view.x2 -= factor*(view.x2-view.x1);
    view.y1 += factor*(view.y2-view.y1);
    view.y2 -= factor*(view.y2-view.y1);
    std::cout << view;
}

void BasicDrawPane::HandleKey(wxKeyEvent &evt) {
    switch (evt.GetKeyCode()) {
        case 315:
        case 'W':
            view.y1 -= 0.05*(view.y2-view.y1);
            view.y2 -= 0.05*(view.y2-view.y1);
            break;
        case 317:
        case 'S':
            view.y1 += 0.05*(view.y2-view.y1);
            view.y2 += 0.05*(view.y2-view.y1);
            break;
        case 314:
        case 'A':
            view.x1 -= 0.05*(view.x2-view.x1);
            view.x2 -= 0.05*(view.x2-view.x1);
            break;
        case 316:
        case 'D':
            view.x1 += 0.05*(view.x2-view.x1);
            view.x2 += 0.05*(view.x2-view.x1);
            break;
    }
    BasicDrawPane::Refresh();
}

class MyFrame : public wxFrame
{
public:
    MyFrame();
private:
    BasicDrawPane * drawPane;
};

MyFrame::MyFrame() : wxFrame(nullptr, 10, "Fractals",wxPoint(50,50), wxSize(600,400)) {
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    drawPane = new BasicDrawPane( (wxFrame*) this);

    sizer->Add(drawPane, 1, wxEXPAND);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);
}

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    auto *frame = new MyFrame();
    frame->Show(true);
    return true;
}