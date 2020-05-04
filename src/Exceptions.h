#pragma once

namespace Application {
    enum Exception {
        COPY_NONTARGET_TEXTURE = 1
        , COMPONENT_NO_EXIST = 2
        , COMPONENT_ALREADY_EXISTS = 3
        , REQUIREMENTS_NOT_FULFILLED = 4
        , GJK_TOO_MANY_VERTICES = 5
        , DEALLOCATE_UNSTARTED_GO = 6
        , COPY_COMP_NO_DUPE = 7
        , INVALID_COL_LAYER = 8
    };
}