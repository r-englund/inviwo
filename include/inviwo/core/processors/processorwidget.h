#ifndef IVW_PROCESSORWIDGET_H
#define IVW_PROCESSORWIDGET_H

#include "inviwo/core/inviwocoredefine.h"
#include "inviwo/core/inviwo.h"
#include "inviwo/core/metadata/processorwidgetmetadata.h"

namespace inviwo {

class Processor;
class IVW_CORE_API ProcessorWidget {

public:
    ProcessorWidget(Processor* processor);
    virtual ~ProcessorWidget();

    virtual void initialize();
    virtual void show();
    virtual void hide();    
    virtual void resize(ivec2);
    virtual void move(ivec2);

private:
    ProcessorWidgetMetaData* metaData_;

protected:
    Processor* processor_;
    bool initialized_; 

    bool getVisibilityMetaData();
    ivec2 getPositionMetaData();
    ivec2 getDimensionMetaData();
};

} // namespace

#endif // IVW_PROCESSORWIDGET_H
