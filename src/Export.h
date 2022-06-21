//
// Created by Georg Huber on 01.06.22.
//

#ifndef FRACTALS_EXPORT_H
#define FRACTALS_EXPORT_H

#include "Vector.h"
#include "Algorithm.h"
#include <wx/string.h>
#include <wx/image.h>


namespace Export {

    void exportImage(Algorithm::Algo o,Vector2D<int> resolution, Vector2x2<long double> view, int depth, wxString filename);
    void exportImages(Algorithm::Algo o,Vector2D<int> resolution, Vector2x2<long double> viewStart, Vector2x2<long double> viewEnd, int depth,int nrOfFrames, wxString path);
};


#endif //FRACTALS_EXPORT_H
