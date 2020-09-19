//
//  TTProcess.h
//  TTMedia
//
//  Created by liang on 18/8/17.
//  Copyright © 2017年 tina. All rights reserved.
//

#ifndef TTProcess_h
#define TTProcess_h

#include "TTFrame.hpp"

#include "TTFilter.hpp"
#include "TTFilterGroup.hpp"
#include "TTTwoPassFilter.hpp"

#include "TTY420ToRGBFilter.hpp"
#include "TTNV12ToRGBFilter.hpp"
#include "TTRGBTextureFilter.hpp"

// color
#include "TTBrightnessFilter.hpp"
#include "TTContrastFilter.hpp"
#include "TTSaturationFilter.hpp"
#include "TTExposureFilter.hpp"
#include "TTHueFilter.hpp"
#include "TTLevelFilter.hpp"
#include "TTWhiteBalanceFilter.hpp"
#include "TTColorMatrixFilter.hpp"
#include "TTSepiaFilter.hpp"
#include "TTGrayscaleFilter.hpp"

// image
#include "TTGaussianBlurFilter.hpp"
#include "TTBilateralFilter.hpp"
#include "TTSingleComponentGaussianBlurFilter.hpp"
#include "TTDirectionalSobelEdgeDetectionFilter.hpp"
#include "TTDirectionalNonmaximumSuppressionFilter.hpp"
#include "TTWeakPixelInclusionFilter.hpp"
#include "TTCannyEdgeDetectionFilter.hpp"

// effect
#include "TTBuffingFilter.hpp"
#include "TTBeautifyFilter.hpp"
#include "TTEmbossFilter.hpp"



#endif /* TTProcess_h */
