#pragma once

#include "dxrt_prerequisites.hpp"

namespace dxrt
{
    class F_render_path;
    class F_scene_render_view;
    
    class A_example : public A_object
    {
    private:
        static TK<A_example> instance_p_;

    public:
        static NCPP_FORCE_INLINE const auto& instance_p()
        {
            return (TKPA_valid<A_example>)instance_p_;
        }
        
    private:
        G_wstring name_;
        TU<F_application> application_p_;
        
        TU<F_render_path> render_path_p_;
        
        TU<F_level> level_p_;
        
        TK<F_actor> spectator_actor_p_;
        TK<F_transform_node> spectator_transform_node_p_;
        TK<F_camera> spectator_camera_p_;
        TK<F_spectator> spectator_p_;
        TK<F_scene_render_view> spectator_render_view_p_;
        
    public:
        NCPP_FORCE_INLINE const auto& name() const noexcept
        {
            return name_;
        }
        NCPP_FORCE_INLINE const auto& application_p() const noexcept
        {
            return application_p_;
        }
        
        NCPP_FORCE_INLINE const auto& render_path_p() const noexcept
        {
            return render_path_p_;
        }
        
        NCPP_FORCE_INLINE const auto& level_p() const noexcept
        {
            return level_p_;
        }
        
        NCPP_FORCE_INLINE const auto& spectator_actor_p() const noexcept
        {
            return spectator_actor_p_;
        }
        NCPP_FORCE_INLINE const auto& spectator_transform_node_p() const noexcept
        {
            return spectator_transform_node_p_;
        }
        NCPP_FORCE_INLINE const auto& spectator_camera_p() const noexcept
        {
            return spectator_camera_p_;
        }
        NCPP_FORCE_INLINE const auto& spectator_p() const noexcept
        {
            return spectator_p_;
        }
        NCPP_FORCE_INLINE const auto& spectator_render_view_p() const noexcept
        {
            return spectator_render_view_p_;
        }

    protected:
        A_example(const G_wstring& name = NCPP_TEXT("None"));

    public:
        virtual ~A_example();

    public:
        void start();

    protected:
        virtual F_application_desc generate_application_desc();
        virtual TU<F_application> create_application();
        virtual void setup_application();
        virtual void start_application();
        virtual void release_application();

    protected:
        virtual void on_application_startup();
        virtual void on_application_shutdown();
        virtual void on_application_gameplay_tick();
        virtual void on_application_render_tick();

    protected:
        virtual TU<F_render_path> create_render_path();
        
    public:
        virtual TK_valid<F_scene_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p);

    public:
        virtual void RG_init_register();
        virtual void RG_early_register();
        virtual void RG_begin_register();
        virtual void RG_end_register();
        virtual void RG_register_view(TKPA_valid<F_scene_render_view>& view_p);

    protected:
        virtual TU<F_level> create_level();
        virtual void setup_level();
        virtual void setup_spectator();
    };
}
