#include "Export.h"
#include <math.h>

void Export::exportImage(Algorithm::Algo o,Vector2D<int> resolution, Vector2x2<long double> view, int depth, wxString filename,Vector2D<double> colouring) {
    auto* im_data = (unsigned char*) malloc(resolution.x * resolution.y * 3 );
    Algorithm::Fractal(o,im_data, resolution, depth, view,colouring);
    wxImage m_image = wxImage(resolution.x, resolution.y, im_data, true);
    m_image.SaveFile(filename,wxBITMAP_TYPE_BMP);
}

/**
 * This function renders a certain number of pictures, with a zoom
 * @param resolution defines the resolution that is rendered
 * @param viewStart defines the starting view
 * @param viewEnd defines the view it should zoom to
 * @param depth defines the depth of the calculation
 * @param nrOfFrames defines the number of Frames
 * @param path defines the path the images are rendered to
 */
void Export::exportImages(Algorithm::Algo o,Vector2D<int> resolution, Vector2x2<long double> viewStart, Vector2x2<long double> viewEnd, int depth,int nrOfFrames, wxString path,Vector2D<double> colouring){
    Vector2x2<long double> view = viewStart;
    Vector2D<long double> viewCenterEnd((viewEnd.x1 + viewEnd.x2)/2.0L, (viewEnd.y1 + viewEnd.y2)/2.0L);

    for (int i = 0; i < nrOfFrames; ++i) {
        Export::exportImage(o,resolution,view,depth,(wxString::Format("%s/%d.bmp",path,i)),colouring);
        view = Algorithm::zoom(view,viewCenterEnd,0.99L,1.0L/nrOfFrames);
        std::cout << "Iteration: " << i+1 << " / " << nrOfFrames << std::endl;
    }
}
