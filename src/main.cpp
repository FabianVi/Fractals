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

    Vector2D<int> photoRes;
    Vector2D<int> videoRes;

    int photoDepth;
    int videoDepth;

    int videoFPS;
    int videoDuration;

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
                                                colouring(Vector2D<double>(20,60)),
                                                photoRes(Vector2D<int>(600,400)),
                                                videoRes(Vector2D<int>(600,400)),
                                                videoDepth(100),
                                                photoDepth(100),
                                                videoFPS(24),
                                                videoDuration(1)
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
    wxMenu *photoSubResMenu;
    wxMenu *photoSubDepthMenu;

    wxMenu *videoMenu;
    wxMenu *videoSubResMenu;
    wxMenu *videoSubDepthMenu;
    wxMenu *videoSubFramerateMenu;

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
    void OnPhotoRes(wxCommandEvent& event);
    void OnPhotoDepth(wxCommandEvent& event);

    void OnSetStart(wxCommandEvent& WXUNUSED(event));
    void OnSetEnd(wxCommandEvent& WXUNUSED(event));
    void OnReset(wxCommandEvent& WXUNUSED(event));
    void OnRenderVideo(wxCommandEvent& WXUNUSED(event));
    void OnVideoRes(wxCommandEvent& event);
    void OnVideoDepth(wxCommandEvent& event);
    void OnVideoFramerate(wxCommandEvent& event);
    void OnVideoDuration(wxCommandEvent& WXUNUSED(event));

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
    photoSubResMenu = new wxMenu;
    photoSubDepthMenu = new wxMenu;
    videoMenu = new wxMenu;
    videoSubResMenu = new wxMenu;
    videoSubDepthMenu = new wxMenu;
    videoSubFramerateMenu = new wxMenu;

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


    photoSubResMenu->AppendRadioItem(451,"&600x400");
    photoSubResMenu->AppendRadioItem(452,"&1200x800");
    photoSubResMenu->AppendRadioItem(453,"&2400x1600");

    photoSubDepthMenu->AppendRadioItem(461,"&100");
    photoSubDepthMenu->AppendRadioItem(462,"&256");
    photoSubDepthMenu->AppendRadioItem(463,"&1000");
    photoSubDepthMenu->AppendRadioItem(464,"&10000");
    photoSubDepthMenu->AppendRadioItem(465,"&100000");

    photoMenu->Append(450,"&Resolution",photoSubResMenu);
    photoMenu->Append(460,"&Depth",photoSubDepthMenu);
    photoMenu->Append(400, wxT("Render Photo \tR"));
    menubar->Append(photoMenu,wxT("&Photo"));

    Connect(400, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnRenderPhoto));
    Connect(451, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoRes));
    Connect(452, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoRes));
    Connect(453, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoRes));
    Connect(461, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoDepth));
    Connect(462, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoDepth));
    Connect(463, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoDepth));
    Connect(464, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoDepth));
    Connect(465, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPhotoDepth));

    videoSubResMenu->AppendRadioItem(551,"&600x400");
    videoSubResMenu->AppendRadioItem(552,"&1200x800");
    videoSubResMenu->AppendRadioItem(553,"&2400x1600");

    videoSubDepthMenu->AppendRadioItem(561,"&100");
    videoSubDepthMenu->AppendRadioItem(562,"&256");
    videoSubDepthMenu->AppendRadioItem(563,"&1000");
    videoSubDepthMenu->AppendRadioItem(564,"&10000");
    videoSubDepthMenu->AppendRadioItem(565,"&100000");

    videoSubFramerateMenu->AppendRadioItem(571,"&24 FPS");
    videoSubFramerateMenu->AppendRadioItem(572,"&30 FPS");
    videoSubFramerateMenu->AppendRadioItem(573,"&60 FPS");

    videoMenu->Append(500, wxT("&Set Start"));
    videoMenu->Append(501, wxT("&Set End"));
    videoMenu->Append(502, wxT("&Reset"));
    videoMenu->Append(550,wxT("&Resolution"),videoSubResMenu);
    videoMenu->Append(560,wxT("&Depth"),videoSubDepthMenu);
    videoMenu->Append(570,wxT("&Framerate"),videoSubFramerateMenu);
    videoMenu->Append(580,wxT("&Duration"));
    videoMenu->Append(503, wxT("&Render Video \tV"));
    menubar->Append(videoMenu, wxT("&Video"));


    Connect(500, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetStart));
    Connect(501, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnSetEnd));
    Connect(502, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnReset));
    Connect(503, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnRenderVideo));
    Connect(551, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoRes));
    Connect(552, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoRes));
    Connect(553, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoRes));
    Connect(561, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoDepth));
    Connect(562, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoDepth));
    Connect(563, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoDepth));
    Connect(564, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoDepth));
    Connect(565, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoDepth));
    Connect(571, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoFramerate));
    Connect(572, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoFramerate));
    Connect(573, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoFramerate));
    Connect(580, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnVideoDuration));


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
    Export::exportImage(drawPane->o,drawPane->photoRes,drawPane->view,drawPane->photoDepth,"../export.bmp", drawPane->colouring);
}

void MyFrame::OnPhotoRes(wxCommandEvent &event) {
    switch (event.GetId()) {
        case 451:
            drawPane->photoRes = Vector2D<int>(600,400);
            std::cout << "600x400" << std::endl;
            break;
        case 452:
            drawPane->photoRes = Vector2D<int>(1200,800);
            std::cout << "1200x800" << std::endl;
            break;
        case 453:
            drawPane->photoRes = Vector2D<int>(2400,1600);
            std::cout << "2400x1600" << std::endl;
            break;
    }
}

void MyFrame::OnPhotoDepth(wxCommandEvent &event) {
    switch (event.GetId()) {
        case 461:
            drawPane->photoDepth = 100;
            std::cout << "100" << std::endl;
            break;
        case 462:
            drawPane->photoDepth = 256;
            std::cout << "256" << std::endl;
            break;
        case 463:
            drawPane->photoDepth = 1000;
            std::cout << "1000" << std::endl;
            break;
        case 464:
            drawPane->photoDepth = 10000;
            std::cout << "10000" << std::endl;
            break;
        case 465:
            drawPane->photoDepth = 100000;
            std::cout << "100000" << std::endl;
            break;
    }
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
    drawPane->start = Vector2x2<long double>(0, 0, 0, 0);
    drawPane->end = Vector2x2<long double>(0, 0, 0, 0);
}

void MyFrame::OnVideoRes(wxCommandEvent &event) {
    switch (event.GetId()) {
        case 551:
            drawPane->videoRes = Vector2D<int>(600,400);
            std::cout << "600x400" << std::endl;
            break;
        case 552:
            drawPane->videoRes = Vector2D<int>(1200,800);
            std::cout << "1200x800" << std::endl;
            break;
        case 553:
            drawPane->videoRes = Vector2D<int>(2400,1600);
            std::cout << "2400x1600" << std::endl;
            break;
    }
}

void MyFrame::OnVideoDepth(wxCommandEvent &event) {
    switch (event.GetId()) {
        case 561:
            drawPane->videoDepth = 100;
            std::cout << "100" << std::endl;
            break;
        case 562:
            drawPane->videoDepth = 256;
            std::cout << "256" << std::endl;
            break;
        case 563:
            drawPane->videoDepth = 1000;
            std::cout << "1000" << std::endl;
            break;
        case 564:
            drawPane->videoDepth = 10000;
            std::cout << "10000" << std::endl;
            break;
        case 565:
            drawPane->videoDepth = 100000;
            std::cout << "100000" << std::endl;
            break;
    }
}

void MyFrame::OnVideoFramerate(wxCommandEvent &event) {
    switch (event.GetId()) {
        case 571:
            drawPane->videoFPS = 24;
            std::cout << "24" << std::endl;
            break;
        case 572:
            drawPane->videoFPS = 30;
            std::cout << "30" << std::endl;
            break;
        case 573:
            drawPane->videoFPS = 60;
            std::cout << "60" << std::endl;
            break;
    }
}

void MyFrame::OnVideoDuration(wxCommandEvent &event) {
    drawPane->videoDuration = wxGetNumberFromUser(
            wxString("Enter duration in seconds"),
            wxString("Duration:"),
            wxString("Duration"),
            10,
            1, 10000
    );

    drawPane->Refresh();
}

void MyFrame::OnRenderVideo(wxCommandEvent& WXUNUSED(event))
{
    Export::exportImages(drawPane->o,drawPane->videoRes,drawPane->start,drawPane->end,drawPane->videoDepth,(drawPane->videoFPS)*(drawPane->videoDuration),"../Images",drawPane->colouring);
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