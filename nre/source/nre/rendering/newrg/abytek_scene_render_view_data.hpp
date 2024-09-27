#pragma once


#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct NCPP_ALIGN(256) F_abytek_scene_render_view_data
    {
        F_matrix4x4_f32 world_to_view_matrix;
        F_matrix4x4_f32 view_to_world_matrix;
        F_matrix4x4_f32 view_to_clip_matrix;
        F_matrix4x4_f32 clip_to_view_matrix;
        F_vector4_f32 frustum_planes[4];
    };
}