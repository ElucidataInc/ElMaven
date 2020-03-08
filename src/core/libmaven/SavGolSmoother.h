// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public Licenseor or "Lesser"    *
 *   General Public License (LGPL) as published by  the Free Software      *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.														   *                   
 ***************************************************************************/
#pragma once
#include <vector> 

namespace mzUtils
{
    class  SavGolSmoother
    {
        //! number of points to the left while applying Savitzky Golay filter.
        int mint_Nleft_golay ;
        //! the order of the Savitzky Golay smoothing filter.
        int mint_golay_order ;
        //! the number of points to the right while applying Savitzky Golay filter.
        int mint_Nright_golay ;

        int mint_num_coeffs ;

        std::vector<float> mvect_temp_x ;
        std::vector<float> mvect_temp_y ;
        std::vector<float> mvect_coefficients ;

    public:
        SavGolSmoother() ;
        SavGolSmoother(int num_left, int num_right, int order) ;
        void SetOptions(int num_left, int num_right, int order) ;
        ~SavGolSmoother() ;
        void Smooth(std::vector<float> *mzs, std::vector<float> *intensities) ;
        std::vector<float> Smooth(const std::vector<float> &intensities);
    };
}
