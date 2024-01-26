/**
 * rect.c: rectangle functions
 */
#include "window/rect.h"

/**
 * @brief clip a rectangle based on boundries
 * @param clip
 * @param boundry
 */
void rect_clip(Rect *clip,Rect *boundry) {
    if(clip) {
        if(boundry) {
            if(clip->_tx<boundry->_tx) {
                clip->_tx=boundry->_tx;
            }
            if(clip->_ty<boundry->_ty) {
                clip->_ty=boundry->_ty;
            }
            if(clip->_bx>boundry->_bx) {
                clip->_bx=boundry->_bx;
            }
            if(clip->_by>boundry->_by) {
                clip->_by=boundry->_by;
            }
        }
        else {
            log_e(_enull);
        }
    }
    else {
        log_e(_enull);
    }
}
