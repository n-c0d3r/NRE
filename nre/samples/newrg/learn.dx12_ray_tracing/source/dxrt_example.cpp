#include "dxrt_example.hpp"
#include "dxrt_render_path.hpp"

namespace dxrt
{
    TK<A_example> A_example::instance_p_;
    
    A_example::A_example(const G_wstring& name) :
        name_(name)
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    A_example::~A_example()
    {
    }

    void A_example::start()
    {
        application_p_ = create_application();
        setup_application();
        start_application();
    }

    F_application_desc A_example::generate_application_desc()
    {
        return F_application_desc {
            .main_surface_desc = {
                .title = name_,
                .size = { 1024, 700 }
            }
        };
    }
    TU<F_application> A_example::create_application()
    {
        return TU<F_application>()(generate_application_desc());
    }
    void A_example::setup_application()
    {
        render_path_p_ = create_render_path();

        level_p_ = create_level();
        setup_level();

        NRE_APPLICATION_STARTUP(application_p_)
        {
            on_application_startup();
        };
        NRE_APPLICATION_SHUTDOWN(application_p_)
        {
            on_application_shutdown();
        };
        NRE_APPLICATION_GAMEPLAY_TICK(application_p_)
        {
            on_application_gameplay_tick();
        };
        NRE_APPLICATION_RENDER_TICK(application_p_)
        {
            on_application_render_tick();
        };
    }
    void A_example::start_application()
    {
        application_p_->start();
    }
    void A_example::release_application()
    {
        application_p_.reset();
    }

    void A_example::on_application_startup()
    {
    }
    void A_example::on_application_shutdown()
    {
        level_p_.reset();
        
        render_path_p_.reset();
    }
    void A_example::on_application_gameplay_tick()
    {
        render_path_p_->update_ui();
    }
    void A_example::on_application_render_tick()
    {
    }

    TU<F_render_path> A_example::create_render_path()
    {
        return TU<F_render_path>()();
    }
    TK_valid<F_scene_render_view> A_example::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return actor_p->T_add_component<F_scene_render_view>();
    }

    void A_example::RG_init_register()
    {
    }
    void A_example::RG_early_register()
    {
    }
    void A_example::RG_begin_register()
    {
    }
    void A_example::RG_end_register()
    {
    }
    void A_example::RG_register_view(TKPA_valid<F_scene_render_view>& view_p)
    {
    }

    TU<F_level> A_example::create_level()
    {
        return TU<F_level>()();
    }
    void A_example::setup_level()
    {
        setup_spectator();
    }
    void A_example::setup_spectator()
    {
        spectator_actor_p_ = level_p_->T_create_actor().no_requirements();
	    spectator_transform_node_p_ = spectator_actor_p_->template T_add_component<F_transform_node>().no_requirements();
	    spectator_camera_p_ = spectator_actor_p_->template T_add_component<F_camera>().no_requirements();
	    spectator_p_ = spectator_actor_p_->template T_add_component<F_spectator>().no_requirements();
        spectator_render_view_p_ = spectator_camera_p_->render_view_p().T_cast<F_scene_render_view>().no_requirements();

        spectator_p_->position = F_vector3 { 0.0f, 0.0f, -10.0f };
        spectator_p_->euler_angles = F_vector3 { 0.45f, 0.0f, 0.0f };
        spectator_p_->move_speed = 4.0f;

        spectator_render_view_p_->bind_output(NRE_MAIN_SWAPCHAIN());
    }
}
