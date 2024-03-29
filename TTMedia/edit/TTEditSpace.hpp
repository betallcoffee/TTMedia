//
//  TTEditSpace.hpp
//  TTMedia
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTEditSpace_hpp
#define TTEditSpace_hpp

#include <pthread.h>
#include <vector>

#include "TTDef.h"
#include "TTMessageLoop.hpp"
#include "TTFFWriter.hpp"
#include "TTFilterFrame.hpp"
#include "TTMaterial.hpp"
#include "TTClip.hpp"

namespace TT {
    
    enum class EditMessage {
        kProccess,
        kOpen,
        kLoadMore,
        kExportFile,
    };
    
    class EditSpace {
    public:
        EditSpace();
        ~EditSpace();
        
        int materialCount();
        TT_SP(Material) material(int index);
        
        void addMaterial(TT_SP(Material) material);
        void removeMaterial(int index);
        
        typedef std::function<void (void)> Callback;
        void loadMoreForMaterial(std::shared_ptr<Material> media, Callback callback);
        
        void addClip(TT_SP(Clip) clip);
        void removeClip(int index);
        
        void exportFile(std::shared_ptr<URL> url);
        
    private:
        void quit();
        
        bool createWriter();
        void exportAllClips();
        
        TT_SP(MessageLoop) _loop;
        
        std::vector<TT_SP(Material)> _materials;
        std::vector<TT_SP(Clip)> _clips;
        
        pthread_mutex_t _editMutex;
        
        std::shared_ptr<URL> _exportUrl;
        std::shared_ptr<FFWriter> _writer;
    };
}

#endif /* TTEditSpace_hpp */
