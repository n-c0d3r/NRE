#pragma once


#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    struct F_abytek_view_buffer_data
    {
        F_matrix4x4_f32 world_to_view_matrix;
        F_matrix4x4_f32 view_to_world_matrix;
        F_matrix4x4_f32 view_to_clip_matrix;
        F_matrix4x4_f32 clip_to_view_matrix;
        F_vector4_f32 frustum_planes[4];
    };
}