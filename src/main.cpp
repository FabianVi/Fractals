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
private:
    Vector2x2<long double> view;
public:
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

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent), view(-2,-1,1,1), o(Algorithm::Algo::Mandelbrot), colouring(Vector2D<double>(20,60)) , depth(100){
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
                    ,"../test.bmp",colouring);
            break;
        case 'V':
            Export::exportImages(o,Vector2D(900*3,900*2),view,Vector2x2<long double>(-1.9537185760276644789536215629510707003646530210971832275390625L,-2.3630621894364577103364029214455262206673324953953851945698261260986328125e-05L,-1.953718576027664449897003340339551868964917957782745361328125L,-2.363062189434091442072803513209204584466505139062064699828624725341796875e-05L),10000,60 * 60,"../Images",colouring);
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
    wxMenu *renderMenu;
    wxMenu *colourPalletMenu;
    wxMenu *propertyMenu;
    void OnMandelbrot(wxCommandEvent& WXUNUSED(event));
    void OnBurningShip(wxCommandEvent& WXUNUSED(event));
    void OnColouringDefault(wxCommandEvent& WXUNUSED(event));
    void OnColouringDark(wxCommandEvent& WXUNUSED(event));
    void OnColouringLight(wxCommandEvent& WXUNUSED(event));
    void OnColouringFancy(wxCommandEvent& WXUNUSED(event));
    void OnSetDepth(wxCommandEvent& WXUNUSED(event));
};

MyFrame::MyFrame() : wxFrame(nullptr, 10, "Fractals",wxPoint(50,50), wxSize(600,400)) {
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    drawPane = new BasicDrawPane( (wxFrame*) this);

    sizer->Add(drawPane, 1, wxEXPAND);
    this->SetSizer(sizer);
    this->SetAutoLayout(true);

    menubar = new wxMenuBar;
    subMenu = new wxMenu;
    colourPalletMenu = new wxMenu;
    renderMenu = new wxMenu;
    propertyMenu = new wxMenu;

    subMenu->AppendRadioItem(200, wxT("&Mandelbrot"));
    subMenu->AppendRadioItem(201, wxT("&Burning Ship"));
    menubar->Append(subMenu, wxT("&Fractal Type"));

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


    propertyMenu->Append(400, wxT("&set Depth"));
    menubar->Append(propertyMenu, wxT("&Properties"));

    Connect(400, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetDepth));


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

void MyFrame::OnSetDepth(wxCommandEvent& WXUNUSED(event))
{
    drawPane->depth = wxGetNumberFromUser(
    wxString("Enter Depth value"),
    wxString("Depth:"),
    wxString("Depth value"),
    265,
    1,10000
    );

    drawPane->Refresh();
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