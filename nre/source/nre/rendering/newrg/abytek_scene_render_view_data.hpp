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
        F_vector4_f32 frustum_planes[6];
        f32 near_plane;
        f32 far_plane;
        F_vector3_f32 view_position;
        F_vector3_f32 view_right;
        F_vector3_f32 view_up;
        F_vector3_f32 view_forward;
        F_vector2_f32 view_size;
    };
}