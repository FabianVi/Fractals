

#include <math.h>
#include <iostream>

#include "wx/wx.h"
#include "wx/sizer.h"

class BasicDrawPane : public wxPanel
{
public:
    BasicDrawPane(wxFrame* parent);

    void paintEvent(wxPaintEvent & evt);
    void paintNow(int x, int y, int val);

    void render(wxDC& dc, int x, int y, int val);

    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */

DECLARE_EVENT_TABLE()
};

class Fractal{
private:
    int* width;
    int* height;
    double minX;
    double minY;
    double maxX;
    double maxY;
    BasicDrawPane* db;
public:
    Fractal(int* width, int* height, BasicDrawPane* db)
            :width(width),height(height), minX(-2.0), minY(-1.0), maxX(1.0),maxY(1.0),db(db){}

    Fractal(int* width, int* height,void(*draw)(int x, int y, int val))
            :width(width),height(height), minX(-2.0), minY(-1.0), maxX(1.0),maxY(1.0){
    }

    Fractal(int* width, int* height, double minX, double minY, double maxX, double maxY)
            :width(width),height(height), minX(minX), minY(minY), maxX(maxX),maxY(maxY){}

    ~Fractal(){delete width; delete height;}

    void calcAll(){
        for (int xp = 0; xp < *width; ++xp) {
            for (int yp = 0; yp < *height; ++yp) {
                double cRe = map(xp, *width, minX, maxX);
                double cIm = map(yp, *height,minY,maxY);

                double zRe = cRe;
                double zIm = cIm;
                double zRePrev = cRe;
                double zImPrev = cIm;
                int n;
                for (n = 0; n < 255; ++n) {
                    zRePrev = zRe*zRe - zIm*zIm; //zRePrev*zRePrev-zImPrev*zImPrev
                    zImPrev=2*zRe*zIm;

                    zRe=zRePrev+cRe;
                    zIm=zImPrev+cIm;

                    if(sqrt(abs(zRe+zIm)) > 16){
                        break;
                    }
                }
                db->paintNow(xp,yp,n);

            }
        }
    }

private:
    double map(int point, int pixelWidth, double min, double max){
        return (point*((max-min)/pixelWidth) + min);
    }

    void draw(int x, int y, int val);
};




class MyApp: public wxApp
{
    bool OnInit();

    wxFrame *frame;
    BasicDrawPane * drawPane;

public:

};

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello wxDC"), wxPoint(50,50), wxSize(1200,800));

    drawPane = new BasicDrawPane( (wxFrame*) frame );
    sizer->Add(drawPane, 1, wxEXPAND);

    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);

    frame->Show();




    return true;
}

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
// some useful events
/*
 EVT_MOTION(BasicDrawPane::mouseMoved)
 EVT_LEFT_DOWN(BasicDrawPane::mouseDown)
 EVT_LEFT_UP(BasicDrawPane::mouseReleased)
 EVT_RIGHT_DOWN(BasicDrawPane::rightClick)
 EVT_LEAVE_WINDOW(BasicDrawPane::mouseLeftWindow)
 EVT_KEY_DOWN(BasicDrawPane::keyPressed)
 EVT_KEY_UP(BasicDrawPane::keyReleased)
 EVT_MOUSEWHEEL(BasicDrawPane::mouseWheelMoved)
 */

// catch paint events
                EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()


// some useful events
/*
 void BasicDrawPane::mouseMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseDown(wxMouseEvent& event) {}
 void BasicDrawPane::mouseWheelMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseReleased(wxMouseEvent& event) {}
 void BasicDrawPane::rightClick(wxMouseEvent& event) {}
 void BasicDrawPane::mouseLeftWindow(wxMouseEvent& event) {}
 void BasicDrawPane::keyPressed(wxKeyEvent& event) {}
 void BasicDrawPane::keyReleased(wxKeyEvent& event) {}
 */

BasicDrawPane::BasicDrawPane(wxFrame* parent) :
        wxPanel(parent)
{

}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void BasicDrawPane::paintEvent(wxPaintEvent & evt)
{
    int width = 1200, height = 800;
    Fractal* f = new Fractal(&width,&height,this);
    f->calcAll();
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
void BasicDrawPane::paintNow(int x, int y, int val)
{
    wxClientDC dc(this);
    render(dc, x,y,val);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void BasicDrawPane::render(wxDC&  dc, int x, int y, int val)
{
    dc.SetPen( wxPen( wxColor(255-val,255-val,255-val), 3 ) ); // black line, 3 pixels thick
    dc.DrawPoint(x,y);
}

