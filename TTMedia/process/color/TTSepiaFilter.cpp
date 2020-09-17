//
//  TTSepiaFilter.cpp
//  TTMedia
//
//  Created by liang on 22/1/20.
//  Copyright © 2020年 tina. All rights reserved.
//

#include "TTSepiaFilter.hpp"

using namespace TT;

SepiaFilter::SepiaFilter() {
    _colorMatrix = (GPUMatrix4x4){
        {0.3588, 0.7044, 0.1368, 0.0},
        {0.2990, 0.5870, 0.1140, 0.0},
        {0.2392, 0.4696, 0.0912 ,0.0},
        {0,0,0,1.0},
    };
}
