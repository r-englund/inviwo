#include <inviwo/core/interaction/trackball.h>
#include <modules/opengl/inviwoopengl.h> // FOR DEBUGGING

namespace inviwo {

static const float RADIUS = 0.5f;

Trackball::Trackball(CameraProperty* camera)
    : InteractionHandler(),
      camera_(camera),
      lastMousePos_(ivec2(0)),
      lastTrackballPos_(vec3(0.5f)),
      pixelWidth_(0.007f),
      isMouseBeingPressedAndHold_(false)
{ 
    keymapper_ = new TrackballKeyMapper();
}

Trackball::~Trackball() {}

vec3 Trackball::mapNormalizedMousePosToTrackball(vec2 mousePos) {
    // set x and y to lie in interval [-r, r]
    float r = RADIUS;
    vec3 result = vec3(mousePos.x-RADIUS, -1.0f*(mousePos.y-RADIUS), 0.0f);
    
    // HOLROYD
    //Piece-wise sphere + hyperbolic sheet        
    if ((result.x*result.x + result.y*result.y) <= r*r/(2.0f)) {
        //Spherical Region
        result.z = r*r - (result.x*result.x + result.y*result.y);
        result.z = result.z > 0.0f ? sqrtf(result.z) : 0.0f;
    } else {
        //Hyperbolic Region - for smooth z values
        result.z = ((r*r)/(2.0f*sqrtf(result.x*result.x + result.y*result.y)));            
    }
    return glm::normalize(result);
}

vec3 Trackball::mapToCamera(vec3 pos) {
    //TODO: Use proper co-ordinate transformation matrices
    //Get x,y,z axis vectors of current camera view
    vec3 currentViewYaxis = glm::normalize(camera_->lookUp());
    vec3 currentViewZaxis = glm::normalize(camera_->lookTo()-camera_->lookFrom());
    vec3 currentViewXaxis = glm::normalize(glm::cross(currentViewYaxis, currentViewZaxis)); 

    //mapping to camera co-ordinate 
    currentViewXaxis*=pos.x;
    currentViewYaxis*=pos.y;
    currentViewZaxis*=pos.z;

    return (currentViewXaxis+currentViewYaxis+currentViewZaxis);
}


void Trackball::invokeEvent(Event* event) {
    MouseEvent* mouseEvent = dynamic_cast<MouseEvent*>(event);

    // FOR DEBUGGING 
    // Draws the camera direction. Should only be seen as a single white pixel in a perfect world
    //glBegin(GL_LINES);
    //    glColor3f(1.0f,1.0f,1.0f);
    //    glVertex3f(camera_->lookTo().x, camera_->lookTo().y, camera_->lookTo().z);
    //    glVertex3f(camera_->lookFrom().x, camera_->lookFrom().y, camera_->lookFrom().z);
    //glEnd();

    if (mouseEvent) {
        if (mouseEvent->button() == keymapper_->getKey(TrackballAction::TRACKBALL_ROTATE) && mouseEvent->state() == MouseEvent::MOUSE_STATE_PRESS) {
            //perform rotation
            rotateCamera(mouseEvent);
        } else if (mouseEvent->button() == MouseEvent::MOUSE_BUTTON_RIGHT && mouseEvent->state() == MouseEvent::MOUSE_STATE_PRESS) {
            //perform zoom
            zoomCamera(mouseEvent);
	    } else if (mouseEvent->button() == MouseEvent::MOUSE_BUTTON_MIDDLE && mouseEvent->state() == MouseEvent::MOUSE_STATE_PRESS) {
            //perform pan
            panCamera(mouseEvent);  
        } else if (mouseEvent->state() == MouseEvent::MOUSE_STATE_RELEASE) {
            isMouseBeingPressedAndHold_ = false;
        } 
        return;
    }       

    ResizeEvent* resizeEvent = dynamic_cast<ResizeEvent*>(event);
    if (resizeEvent) {
        uvec2 canvasSize = resizeEvent->canvasSize();
        float width = (float) canvasSize[0];
        float height = (float) canvasSize[1];
        camera_->setProjectionMatrix(60.f, width/height, 0.0001f, 100.0f );
        return;
    }    
}

void Trackball::rotateCamera(MouseEvent* mouseEvent) {
    ivwAssert(mouseEvent!=0, "Invalid mouse event.");
    // ROTATION
    vec2 curMousePos = mouseEvent->posNormalized();
    vec3 curTrackballPos = mapNormalizedMousePosToTrackball(curMousePos);
    float lookLength;

    // disable movements on first press
    if (!isMouseBeingPressedAndHold_) {				
        lastTrackballPos_ = curTrackballPos;
        lastMousePos_ = curMousePos;
        isMouseBeingPressedAndHold_ = true;
    }

    if (curTrackballPos != lastTrackballPos_) {    

        // calculate rotation angle (in degrees)
        float rotationAngle = glm::angle(curTrackballPos, lastTrackballPos_);

        //difference vector in trackball co-ordinates
        vec3 trackBallOffsetVector = lastTrackballPos_ - curTrackballPos;        

        //compute next camera position
        vec3 mappedTrackBallOffsetVector = mapToCamera(trackBallOffsetVector);
        vec3 currentCamPos = camera_->lookTo();        
        vec3 nextCamPos = currentCamPos + mappedTrackBallOffsetVector;

        // FOR DEBUGGING 
        // Draws a line with the trackball x and y. Color with z.
        //glBegin(GL_LINES);
        //    glColor3f(1.0f, curTrackballPos.z, 0.0f);
        //    glVertex3f(0.0f,0,0.0f);
        //    glVertex3f(curTrackballPos.x, curTrackballPos.y, 0.0f);
        //glEnd();

        // obtain rotation axis
        if(rotationAngle > pixelWidth_) {
            //rotation axis
            vec3 rotationAxis = glm::cross(currentCamPos, nextCamPos);

            // generate quaternion and rotate camera                
            rotationAxis = glm::normalize(rotationAxis);                
            quat quaternion = glm::angleAxis(rotationAngle, rotationAxis);
            lookLength = glm::length(camera_->lookFrom()-camera_->lookTo());
            vec3 offset = camera_->lookFrom();
            vec3 rotation = glm::rotate(quaternion, offset);
            camera_->setLookFrom(rotation);
            camera_->setLookTo(glm::rotate(quaternion, camera_->lookTo()));
            camera_->setLookUp(glm::rotate(quaternion, camera_->lookUp()));

            // Check the length of the length-vector, might change due to float precision
            if (lookLength != glm::length(camera_->lookFrom()-camera_->lookTo())) {
                float diff = lookLength/glm::length(camera_->lookFrom()-camera_->lookTo());
                camera_->setLookTo(camera_->lookTo()*diff);
            }

            camera_->invalidate();

            //update mouse positions
            lastMousePos_ = curMousePos;
            lastTrackballPos_ = curTrackballPos;
        }
    }

    return;
}

void Trackball::zoomCamera(MouseEvent* mouseEvent) {
    ivwAssert(mouseEvent!=0, "Invalid mouse event.");

    // ZOOM
    float diff;
    vec2 curMousePos = mouseEvent->posNormalized();
    vec3 curTrackballPos = mapNormalizedMousePosToTrackball(curMousePos);

    // compute direction vector
    vec3 direction = camera_->lookFrom() - camera_->lookTo();

    // disable movements on first press
    if (!isMouseBeingPressedAndHold_){				
        lastMousePos_ = curMousePos;
        lastTrackballPos_ = curTrackballPos;
        isMouseBeingPressedAndHold_ = true;
    }

    if (curMousePos != lastMousePos_ && direction.length() > 0) {

        // use the difference in mouse y-position to determine amount of zoom				
        diff = curTrackballPos.y - lastTrackballPos_.y;

        // zoom by moving the camera
        camera_->setLookTo(camera_->lookTo()+direction*diff);
        camera_->invalidate();
        lastMousePos_ = curMousePos;
        lastTrackballPos_ = curTrackballPos;
    }

    return;
}

void Trackball::panCamera(MouseEvent* mouseEvent) {
    ivwAssert(mouseEvent!=0, "Invalid mouse event.");

    // PAN
    vec2 curMousePos = mouseEvent->posNormalized();
    vec3 curTrackballPos = mapNormalizedMousePosToTrackball(curMousePos);

    // disable movements on first press
    if (!isMouseBeingPressedAndHold_) {
        lastMousePos_ = curMousePos;
        lastTrackballPos_ = curTrackballPos;
        isMouseBeingPressedAndHold_ = true;
    }

    //difference vector in trackball co-ordinates
    vec3 trackBallOffsetVector = lastTrackballPos_ - curTrackballPos;        

    //compute next camera position
    trackBallOffsetVector.z = 0.0f;
    vec3 mappedTrackBallOffsetVector = mapToCamera(trackBallOffsetVector);

    if (curMousePos != lastMousePos_) {
        camera_->setLookTo(camera_->lookTo() + mappedTrackBallOffsetVector);
        camera_->setLookFrom(camera_->lookFrom() + mappedTrackBallOffsetVector);
        camera_->invalidate();
        lastMousePos_ = curMousePos;
        lastTrackballPos_ = curTrackballPos;
    }

    return;
}


} // namespace