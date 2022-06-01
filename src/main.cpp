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

#define benchmark
#include "benchmark.h"

class BasicDrawPane : public wxPanel
{
private:
    Vector2x2<long double> view;
public:
    explicit BasicDrawPane(wxFrame* parent);
    void paintEvent(wxPaintEvent & evt);
    void OnClick(wxMouseEvent& evt);
    void Mousewheel(wxMouseEvent& evt);

    void HandleKey(wxKeyEvent& evt);

    void zoom(long double factor);
};

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent), view(-2,-1,1,1){
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
    DestinctTimer draw_timer("draw img");

    int w;
    int h;
    this->GetSize(&w,&h);

    auto* im_data = (unsigned char*) malloc(w * h * 3 );
    Algorithm::Fractal(im_data, Vector2D<int>(w , h), 256, view);

    draw_timer.start();
    wxClientDC dc(this);

    dc.Clear();
    wxImage m_image = wxImage(w, h, im_data, true);
    wxBitmap m_bitmap = wxBitmap(m_image);

    dc.DrawBitmap(m_bitmap,0,0,true);
    draw_timer.stop();
}

void BasicDrawPane::OnClick(wxMouseEvent& evt){
    int x, y;
    evt.GetPosition(&x,&y);

    int w,h;
    this->GetSize(&w,&h);

    long double  x_m=Algorithm::map(x,w,view.x1,view.x2),
                 y_m=Algorithm::map(y,h,view.y1,view.y2);

    view = Algorithm::zoom(view,Vector2D(x_m,y_m) , 0.5, 1.L);

    BasicDrawPane::Refresh();
}

void BasicDrawPane::Mousewheel(wxMouseEvent &evt) {
    int rot = evt.GetWheelRotation();
    long double zoomFactor = rot * 0.001L+1L;

    int x, y;
    evt.GetPosition(&x,&y);

    int w,h;
    this->GetSize(&w,&h);

    long double  x_m=Algorithm::map(x,w,view.x1,view.x2),
                 y_m=Algorithm::map(y,h,view.y1,view.y2);

    view = Algorithm::zoom(view,Vector2D(x_m,y_m) , zoomFactor, 0.L);

   BasicDrawPane::Refresh();
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
    BasicDrawPane* drawPane;
    wxMenuBar* menubar;
    wxMenu *subMenu;
};

MyFrame::MyFrame() : wxFrame(nullptr, 10, "Fractals",wxPoint(50,50), wxSize(600,400)) {
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    drawPane = new BasicDrawPane( (wxFrame*) this);

    sizer->Add(drawPane, 1, wxEXPAND);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);

    menubar = new wxMenuBar;
    subMenu = new wxMenu;

    subMenu->AppendRadioItem(200, wxT("&Mandelbrot"));
    subMenu->AppendRadioItem(201, wxT("&Julia Set"));
    subMenu->AppendRadioItem(202, wxT("&Georgs Gesicht"));
    menubar->Append(subMenu, wxT("&Fractal Type"));

    //   Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
    //   wxCommandEventHandler(SimpleMenu::OnQuit));

    Centre();

    SetMenuBar(menubar);
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