/**
 * rect.h: rectangle functions
 */
#ifndef RECT_H

#   define RECT_H

#   include "os/os.h"

    typedef struct s_Rect {
        int16_t _tx,_ty;
        int16_t _bx,_by;
    } Rect;


#   ifdef __cplusplus
        extern "C" {
#   endif

#   define rect_width(rect)     ((rect)!=0?(rect)->_bx-(rect)->_tx:log_e(_enull))
#   define rect_height(rect)    ((rect)!=0?(rect)->_by-(rect)->_ty:log_e(_enull))
#   define rect_valid(rect)     ((rect)!=0                                     \
                                   ?((rect)->_bx>=(rect)->_tx               \
                                        &&(rect)->_by>=(rect)->_ty)         \
                                   :log_e(_enull))
#   define rect_within(rect,x,y)    ((rect)!=0                                 \
                                        ?x>=(rect)->_tx&&x<(rect)->_bx      \
                                        &&y>=(rect)->_ty&&y<(rect)->_by     \
                                        :log_e(_enull))

    void rect_clip(Rect *clip,Rect *boundry);

#   ifdef __cplusplus
        }
#   endif

#endif  // RECT_H
