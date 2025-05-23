#pragma once

#include <nre/prerequisites.hpp>



namespace nre {

    class F_render_foundation_rg_register_render_primitive_data_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_rg_register_render_primitive_data_event()
        {}
        ~F_render_foundation_rg_register_render_primitive_data_event(){}
    };

    class F_render_foundation_rg_register_render_primitive_data_upload_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_rg_register_render_primitive_data_upload_event()
        {}
        ~F_render_foundation_rg_register_render_primitive_data_upload_event(){}
    };

    class F_render_foundation_unified_mesh_system_rg_end_register_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_unified_mesh_system_rg_end_register_event()
        {}
        ~F_render_foundation_unified_mesh_system_rg_end_register_event(){}
    };
    class F_render_foundation_unified_mesh_system_rg_begin_register_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_unified_mesh_system_rg_begin_register_event()
        {}
        ~F_render_foundation_unified_mesh_system_rg_begin_register_event(){}
    };

    class F_render_foundation_rg_tick_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_rg_tick_event()
        {}
        ~F_render_foundation_rg_tick_event(){}
    };

    class F_render_foundation_rg_pre_execute_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_rg_pre_execute_event()
        {}
        ~F_render_foundation_rg_pre_execute_event(){}
    };

    class F_render_foundation_upload_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_upload_event()
        {}
        ~F_render_foundation_upload_event(){}
    };

    class F_render_foundation_readback_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_readback_event()
        {}
        ~F_render_foundation_readback_event(){}
    };

    class F_render_foundation_release_event : public F_event {

    public:
        NCPP_FORCE_INLINE F_render_foundation_release_event()
        {}
        ~F_render_foundation_release_event(){}
    };
}