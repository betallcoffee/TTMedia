//
//  TTMaterial.cpp
//  TTPlayerExample
//
//  Created by liang on 11/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#include "TTMaterial.hpp"

using namespace TT;

Material::Material(eMaterialType type, std::shared_ptr<URL> url)
: _type(type)
, _url(url)
{
    
}

Material::~Material() {
    
}


