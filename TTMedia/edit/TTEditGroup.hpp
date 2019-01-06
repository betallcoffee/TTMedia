//
//  TTEditGroup.hpp
//  TTPlayerExample
//
//  Created by liang on 10/3/18.
//  Copyright © 2018年 tina. All rights reserved.
//

#ifndef TTEditGroup_hpp
#define TTEditGroup_hpp

#include <pthread.h>
#include <vector>

#include "TTMessageLoop.hpp"
#include "TTFFWriter.hpp"
#include "TTFilterFrame.hpp"
#include "TTMaterial.hpp"
#include "TTVideo.hpp"

namespace TT {
    
    enum class EditMessage {
        kProccess,
        kOpen,
        kLoadMore,
        kExportFile,
    };
    
    class EditGroup {
    public:
        EditGroup();
        ~EditGroup();
        
        int materialCount();
        std::shared_ptr<Material> material(int index);
        
        void addMaterial(std::shared_ptr<Material> material);
        void removeMaterial(int index);
        
        typedef std::function<void (void)> Callback;
        void loadMoreForMaterial(std::shared_ptr<Material> material, Callback callback);
        
        void exportFile(std::shared_ptr<URL> url);
        
    private:
        void quit();
        
        std::shared_ptr<MessageLoop> _loop;
        
        std::vector<std::shared_ptr<Material>> _materials;
        std::vector<std::shared_ptr<Video>> _videos;
        
        pthread_mutex_t _editMutex;
        
        std::shared_ptr<URL> _exportUrl;
        std::shared_ptr<FFWriter> _writer;
    };
}

#endif /* TTEditGroup_hpp */
