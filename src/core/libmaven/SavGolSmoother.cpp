// Written by Navdeep Jaitly for the Department of Energy (PNNL, Richland, WA)
// Copyright 2006, Battelle Memorial Institute
// E-mail: navdeep.jaitly@pnl.gov
// Website: http://ncrr.pnl.gov/software
// -------------------------------------------------------------------------------
// 

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public Licenseor or "Lesser"    *
 *   General Public License (LGPL) as published by  the Free Software      *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.														   *                   
 ***************************************************************************/

#include "SavGolSmoother.h"
#include <iostream>
namespace mzUtils
{

    extern int savgol(float *c, int np, int nl,int nr, int ld, int m) ;

    SavGolSmoother::SavGolSmoother(int num_left, int num_right, int order)
    {
        SetOptions(num_left, num_right, order) ;
    }

    SavGolSmoother::SavGolSmoother()
    {
        SetOptions(3, 3, 2) ;
    }

    SavGolSmoother::~SavGolSmoother(void)
    {
    }

    void SavGolSmoother::SetOptions(int num_left, int num_right, int order)
    {
        mvect_coefficients.clear() ;

        mint_golay_order = order ;
        mint_Nleft_golay = num_left ;
        mint_Nright_golay = num_right ;
        int np = mint_Nleft_golay + mint_Nright_golay + 1 ;

        float *golay_coeffs = new float[np+2] ;

        for (int i = 0 ; i < np+2 ; i++)
            golay_coeffs[i] = 0 ;


        savgol(golay_coeffs, np, mint_Nleft_golay, mint_Nright_golay , 0, mint_golay_order) ;

        mint_num_coeffs = mint_Nright_golay * 2 ;
        if (mint_Nleft_golay > mint_Nright_golay)
        {
            mint_num_coeffs = mint_Nleft_golay * 2 ;
        }

        // unwrap golay coeffs
        mvect_coefficients.clear() ;
        for (int i = 0 ; i < mint_Nleft_golay + mint_Nright_golay + 1 ; i++)
            mvect_coefficients.push_back(0) ;

        for(int i = 0 ; i <= mint_Nleft_golay ; i++)
        {
            mvect_coefficients[mint_num_coeffs/2 - i] = (float) golay_coeffs[i+1] ;
        }
        for (int i = 1 ; i <= mint_Nright_golay ; i++)
        {
            mvect_coefficients[mint_num_coeffs/2+i] = (float) golay_coeffs[mint_num_coeffs-i] ;
        }
        delete [] golay_coeffs ;
    }


    void SavGolSmoother::Smooth(std::vector<float> *mzs, std::vector<float> *intensities)
    {
        //int width = (mint_num_coeffs/2) ;
        int size = (int)mzs->size() ;
        mvect_temp_x.clear() ;
        mvect_temp_y.clear() ;

        for (int i = 0 ; i < size ; i++)
        {
            int start_index = i - mint_Nleft_golay ;
            int stop_index = i + mint_Nright_golay +1 ;

            if (start_index < 0 || stop_index >= size)
            {
                // dont worry about smoothing just push back and forget.
                mvect_temp_x.push_back(mzs->at(i)) ;
                mvect_temp_y.push_back(intensities->at(i)) ;
                continue ;
            }
            float sum = 0 ;
            float sum_before = 0 ;
            for (int j = start_index ; j < stop_index ; j++)
            {
                sum_before = sum ;
                float val = intensities->at(j) * mvect_coefficients[j-start_index]  ;
                sum = sum_before + val ;
            }

            if (sum < 0) sum = 0 ;
            mvect_temp_x.push_back(mzs->at(i)) ;
            mvect_temp_y.push_back(sum) ;
        }
        mzs->clear() ;
        intensities->clear() ;
        mzs->insert(mzs->begin(), mvect_temp_x.begin(), mvect_temp_x.end()) ;
        intensities->insert(intensities->begin(), mvect_temp_y.begin(), mvect_temp_y.end()) ;
    }

    std::vector<float> SavGolSmoother::Smooth(const std::vector<float>& intensities)
    {
        int size = (int) intensities.size() ;
        mvect_temp_y.resize(size);

        for (int i = 0 ; i < size ; i++)
        {
            int start_index = i - mint_Nleft_golay ;
            int stop_index =  i + mint_Nright_golay +1 ;
            mvect_temp_y[i] = 0;

            if (start_index < 0 || stop_index >= size)
            {
                // dont worry about smoothing just push back and forget.
                mvect_temp_y[i] = intensities[i];
                continue ;
            }

            float sum = 0 ;
            float sum_before = 0 ;
            for (int j = start_index ; j < stop_index ; j++)
            {
                sum_before = sum ;
                float val = intensities.at(j) * mvect_coefficients[j-start_index]  ;
                sum = sum_before + val ;
            }

            if (sum < 0) sum = 0 ;
            mvect_temp_y[i]=sum;
        }
       return mvect_temp_y;
    }
}
