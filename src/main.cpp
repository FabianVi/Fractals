#include <cmath>
#include <iostream>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/gdicmn.h>
#include <wx/numdlg.h>

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
    Vector2D<double> colouring;
    int depth;
};

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent),
                                                o(Algorithm::Algo::Mandelbrot),
                                                view(-2,-1,1,1),
                                                start(Vector2x2<long double>(0,0,0,0)),
                                                end(Vector2x2<long double>(0,0,0,0)),
                                                depth(100),
                                                colouring(Vector2D<double>(20,60))
{
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
    Algorithm::Fractal(o,im_data, Vector2D<int>(w , h), depth , view, colouring);

    Algorithm::DrawRect(start,view,im_data,Vector2D<int>(w , h), 255, 0 ,0 );
    Algorithm::DrawRect(end,view,im_data,Vector2D<int>(w , h), 0, 255 , 0 );

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
    wxMenu *colourPalletMenu;
    wxMenu *propertyMenu;

    void OnMandelbrot(wxCommandEvent& WXUNUSED(event));
    void OnBurningShip(wxCommandEvent& WXUNUSED(event));

    void OnColouringDefault(wxCommandEvent& WXUNUSED(event));
    void OnColouringDark(wxCommandEvent& WXUNUSED(event));
    void OnColouringLight(wxCommandEvent& WXUNUSED(event));
    void OnColouringFancy(wxCommandEvent& WXUNUSED(event));

    void OnSetDepth(wxCommandEvent& WXUNUSED(event));

    void OnRenderPhoto(wxCommandEvent& WXUNUSED(event));
    void OnSetStart(wxCommandEvent& WXUNUSED(event));
    void OnSetEnd(wxCommandEvent& WXUNUSED(event));
    void OnReset(wxCommandEvent& WXUNUSED(event));
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
    colourPalletMenu = new wxMenu;
    propertyMenu = new wxMenu;
    photoMenu = new wxMenu;
    videoMenu = new wxMenu;

    typeMenu->AppendRadioItem(200, wxT("&Mandelbrot"));
    typeMenu->AppendRadioItem(201, wxT("&Burning Ship"));
    menubar->Append(typeMenu, wxT("&Fractal Type"));

    Connect(200, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnMandelbrot));
    Connect(201, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnBurningShip));


    colourPalletMenu->AppendRadioItem(300, wxT("&default"));
    colourPalletMenu->AppendRadioItem(301, wxT("&dark"));
    colourPalletMenu->AppendRadioItem(302, wxT("&light"));
    colourPalletMenu->AppendRadioItem(303, wxT("&fancy"));
    menubar->Append(colourPalletMenu, wxT("&Colouring"));

    Connect(300, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnColouringDefault));
    Connect(301, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnColouringDark));
    Connect(302, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnColouringLight));
    Connect(303, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnColouringFancy));


    photoMenu->Append(400, wxT("Render Photo \tR"));
    menubar->Append(photoMenu,wxT("&Photo"));
    Connect(400, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnRenderPhoto));


    videoMenu->Append(500, wxT("&Set Start"));
    videoMenu->Append(501, wxT("&Set End"));
    videoMenu->Append(502, wxT("&Reset"));
    videoMenu->Append(503, wxT("&Render Video \tV"));
    menubar->Append(videoMenu, wxT("&Video"));


    Connect(500, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetStart));
    Connect(501, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetEnd));
    Connect(502, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnReset));
    Connect(503, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnRenderVideo));


    propertyMenu->Append(600, wxT("&set Depth"));
    menubar->Append(propertyMenu, wxT("&Properties"));

    Connect(600, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetDepth));


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

void MyFrame::OnColouringDefault(wxCommandEvent& WXUNUSED(event))
{
    drawPane->colouring.x = 20;
    drawPane->colouring.y = 60;
    drawPane->Refresh();
}

void MyFrame::OnColouringDark(wxCommandEvent& WXUNUSED(event))
{
    drawPane->colouring.x = 0;
    drawPane->colouring.y = 40;
    drawPane->Refresh();
}

void MyFrame::OnColouringLight(wxCommandEvent& WXUNUSED(event))
{
    drawPane->colouring.x = 120;
    drawPane->colouring.y = 60;
    drawPane->Refresh();
}

void MyFrame::OnColouringFancy(wxCommandEvent& WXUNUSED(event))
{
    drawPane->colouring.x = 0;
    drawPane->colouring.y = 360;
    drawPane->Refresh();
}

void MyFrame::OnSetDepth(wxCommandEvent& WXUNUSED(event)) {
    drawPane->depth = wxGetNumberFromUser(
            wxString("Enter Depth value"),
            wxString("Depth:"),
            wxString("Depth value"),
            265,
            1, 10000
    );

    drawPane->Refresh();
}

void MyFrame::OnRenderPhoto(wxCommandEvent& WXUNUSED(event))
{
    int w, h;
    drawPane->GetSize(&w,&h);
    Export::exportImage(drawPane->o,Vector2D<int>(w,h),drawPane->view,drawPane->depth,"./export.bmp", drawPane->colouring);
}

void MyFrame::OnSetStart(wxCommandEvent& WXUNUSED(event))
{
    drawPane->start = drawPane->view;
}

void MyFrame::OnSetEnd(wxCommandEvent& WXUNUSED(event))
{
    drawPane->end = drawPane->view;
}

void MyFrame::OnReset(wxCommandEvent& WXUNUSED(event)) {
    drawPane->start = Vector2x2<long double>(0,0,0,0);
    drawPane->end = Vector2x2<long double>(0,0,0,0);
}

void MyFrame::OnRenderVideo(wxCommandEvent& WXUNUSED(event))
{
    int w, h;
    drawPane->GetSize(&w,&h);
    Export::exportImages(drawPane->o,Vector2D<int>(w,h),drawPane->start,drawPane->end,drawPane->depth,10,"../Images",drawPane->colouring);
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