#ifndef GOS_INPUT_H
#define GOS_INPUT_H

namespace input {

enum KeyState {
    KS_FREE = 0, // in not pressed state
    KS_PRESSED, // just pressed
    KS_HELD,    // in pressed state 
    KS_RELEASED // just released
};


struct MouseInfo {
    MouseInfo();

    static const int NUM_BUTTONS = 5;
    float x_;
    float y_;
    float rel_x_;
    float rel_y_;
    float wheel_vert_;
    float wheel_hor_;
    KeyState button_state_[NUM_BUTTONS];
};

void handleMouseMotion(const SDL_Event* event, MouseInfo* mi);
void handleMouseButton(const SDL_Event* event, MouseInfo* mi);
void handleMouseWheel(const SDL_Event* event, MouseInfo* mi);

void updateMouseState(MouseInfo* mi);

}

#endif //GOS_INPUT_H
