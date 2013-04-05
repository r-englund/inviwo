#ifndef IVW_TRACKBALL_H
#define IVW_TRACKBALL_H

#include <inviwo/core/inviwocoredefine.h>
#include <inviwo/core/interaction/interactionhandler.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <inviwo/core/interaction/trackballkeymapper.h>
#include <inviwo/core/interaction/events/eventlistener.h>
#include <inviwo/core/interaction/events/resizeevent.h>
#include <inviwo/core/properties/cameraproperty.h>

#include <inviwo/core/properties/eventproperty.h>
#include <inviwo/core/interaction/events/event.h>
#include <inviwo/core/interaction/trackballaction.h>


namespace inviwo {

    class IVW_CORE_API Trackball : public InteractionHandler {

    public:
        Trackball(CameraProperty* camera);
        ~Trackball();

        virtual void invokeEvent(Event* event);
        TrackballKeyMapper* getMapper(){return keymapper_;}

    private:

        float pixelWidth_;
        bool isMouseBeingPressedAndHold_;

        vec2 lastMousePos_;
        vec3 lastTrackballPos_;

        CameraProperty* camera_;
        TrackballKeyMapper* keymapper_;

        vec3 mapNormalizedMousePosToTrackball(vec2 mousePos);
        vec3 mapToCamera(vec3 pos);
        void rotateCamera(MouseEvent* mouseEvent);
        void zoomCamera(MouseEvent* mouseEvent);
        void panCamera(MouseEvent* mouseEvent);
    };

} // namespace

#endif // IVW_TRACKBALL_H