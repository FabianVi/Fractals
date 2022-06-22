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

#include "Export.h"

class BasicDrawPane : public wxPanel
{
public:
    Vector2x2<long double> view;
    Vector2x2<long double> start;
    Vector2x2<long double> end;

    explicit BasicDrawPane(wxFrame* parent);
    void paintEvent(wxPaintEvent & evt);
    void OnClick(wxMouseEvent& evt);
    void Mousewheel(wxMouseEvent& evt);

    void HandleKey(wxKeyEvent& evt);

    void zoom(long double factor);
    Algorithm::Algo o;
};

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent), view(-2,-1,1,1), start(view), end(Vector2x2<long double>(-1.9537185760276644789536215629510707003646530210971832275390625L,-2.3630621894364577103364029214455262206673324953953851945698261260986328125e-05L,-1.953718576027664449897003340339551868964917957782745361328125L,-2.363062189434091442072803513209204584466505139062064699828624725341796875e-05L)), o(Algorithm::Algo::Mandelbrot){
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
    Algorithm::Fractal(o,im_data, Vector2D<int>(w , h), 200 , view);

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

    view = Algorithm::zoom(view,Vector2D(x_m,y_m) , 0.5, .5L);
    std::cout << view << std::endl;
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
        case 'R':
            Export::exportImage(o,Vector2D(2000*3,2000*2)
                    , view, 10000
                    ,"../test.bmp");
            break;
        case 'V':
            Export::exportImages(o,Vector2D(900*3,900*2),view,Vector2x2<long double>(-1.9537185760276644789536215629510707003646530210971832275390625L,-2.3630621894364577103364029214455262206673324953953851945698261260986328125e-05L,-1.953718576027664449897003340339551868964917957782745361328125L,-2.363062189434091442072803513209204584466505139062064699828624725341796875e-05L),10000,60 * 60,"../Images");
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
    wxMenu *typeMenu;
    wxMenu *photoMenu;
    wxMenu *videoMenu;

    void OnMandelbrot(wxCommandEvent& WXUNUSED(event));
    void OnBurningShip(wxCommandEvent& WXUNUSED(event));
    void OnRenderPhoto(wxCommandEvent& WXUNUSED(event));
    void OnSetStart(wxCommandEvent& WXUNUSED(event));
    void OnSetEnd(wxCommandEvent& WXUNUSED(event));
    void OnRenderVideo(wxCommandEvent& WXUNUSED(event));
};

MyFrame::MyFrame() : wxFrame(nullptr, 10, "Fractals",wxPoint(50,50), wxSize(600,400)) {
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    drawPane = new BasicDrawPane( (wxFrame*) this);

    sizer->Add(drawPane, 1, wxEXPAND);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);

    menubar = new wxMenuBar;

    typeMenu = new wxMenu;
    photoMenu = new wxMenu;
    videoMenu = new wxMenu;

    typeMenu->AppendRadioItem(200, wxT("&Mandelbrot"));
    typeMenu->AppendRadioItem(201, wxT("&Burning Ship"));
    menubar->Append(typeMenu, wxT("&Fractal Type"));

    photoMenu->Append(300, wxT("Render Photo"));
    menubar->Append(photoMenu,wxT("&Photo"));

    videoMenu->Append(400, wxT("&Set Start"));
    videoMenu->Append(401, wxT("&Set End"));
    videoMenu->Append(402, wxT("&Render Video"));
    menubar->Append(videoMenu, wxT("&Video"));


    Connect(200, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnMandelbrot));
    Connect(201, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnBurningShip));
    Connect(300, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnRenderPhoto));
    Connect(400, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetStart));
    Connect(401, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetEnd));
    Connect(402, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnRenderVideo));

    Centre();

    SetMenuBar(menubar);
}

void MyFrame::OnMandelbrot(wxCommandEvent& WXUNUSED(event))
{
    drawPane->o = Algorithm::Algo::Mandelbrot;
    drawPane->Refresh();
}

void MyFrame::OnBurningShip(wxCommandEvent& WXUNUSED(event))
{
    drawPane->o = Algorithm::Algo::BurningShip;
    drawPane->Refresh();
}

void MyFrame::OnRenderPhoto(wxCommandEvent& WXUNUSED(event))
{
    int w, h;
    drawPane->GetSize(&w,&h);
    Export::exportImage(drawPane->o,Vector2D<int>(w,h),drawPane->view,10000,"./export.bmp");
}

void MyFrame::OnSetStart(wxCommandEvent& WXUNUSED(event))
{
    drawPane->start = drawPane->view;
}

void MyFrame::OnSetEnd(wxCommandEvent& WXUNUSED(event))
{
    drawPane->end = drawPane->view;
}

void MyFrame::OnRenderVideo(wxCommandEvent& WXUNUSED(event))
{
    int w, h;
    drawPane->GetSize(&w,&h);
    Export::exportImages(drawPane->o,Vector2D<int>(w,h),drawPane->start,drawPane->end,10000,10,"../Images");
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