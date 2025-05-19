#include "dxrt_render_path.hpp"

#include "dxrt_example.hpp"


namespace dxrt
{
    
    F_scene_render_view::F_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask) :
        newrg::F_scene_render_view(
            actor_p,
            mask
        )
    {
        NRE_ACTOR_COMPONENT_REGISTER(F_scene_render_view);

        depth_mode = E_render_view_depth_mode::REVERSE;

        actor_p->set_gameplay_tick(true);
    }
    F_scene_render_view::~F_scene_render_view()
    {
    }

    void F_scene_render_view::update_ui()
    {
        ImGui::Begin(
            (
                "DXRT Scene Render View ("
                + actor_p()->name()
                + ")"
            ).c_str()
        );

        auto output_size = size();
        ImGui::Text("Output Size: (%d, %d)", output_size.x, output_size.y);

        ImGui::End();
    }

    void F_scene_render_view::gameplay_tick()
    {
        newrg::F_scene_render_view::gameplay_tick();

        update_ui();
    }
    void F_scene_render_view::render_tick()
    {
        newrg::F_scene_render_view::render_tick();
    }

    void F_scene_render_view::RG_begin_register()
    {
        NCPP_ASSERT(depth_mode == E_render_view_depth_mode::REVERSE) << "E_render_view_depth_mode::REVERSE is required";

        newrg::F_scene_render_view::RG_begin_register();
    }
    void F_scene_render_view::RG_end_register()
    {
        A_example::instance_p()->RG_end_register();
    }
    
    F_render_factory_proxy::F_render_factory_proxy()
    {
    }
    F_render_factory_proxy::~F_render_factory_proxy()
    {
    }

    TK_valid<A_render_view> F_render_factory_proxy::create_scene_render_view(TKPA_valid<F_actor> actor_p)
    {
        return A_example::instance_p()->create_scene_render_view(actor_p);
    }
    
    F_render_path::F_render_path() :
        newrg::F_render_path(TU<F_render_factory_proxy>()())
    {
        //
        min_wave_size_ = NRE_MAIN_DEVICE()->min_wave_size();
        max_wave_size_ = NRE_MAIN_DEVICE()->max_wave_size();
        total_lane_count_ = NRE_MAIN_DEVICE()->total_lane_count();
        
        // setup nsl
        F_nsl_shader_system::instance_p()->define_global_macro({
            "DXRT_DEFAULT_THREAD_GROUP_SIZE_X",
            G_to_string(DXRT_DEFAULT_THREAD_GROUP_SIZE_X)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "DXRT_DEFAULT_THREAD_GROUP_SIZE_Y",
            G_to_string(1)
        });
        F_nsl_shader_system::instance_p()->define_global_macro({
            "DXRT_DEFAULT_THREAD_GROUP_SIZE_Z",
            G_to_string(1)
        });
    }
    F_render_path::~F_render_path()
    {
    }

    void F_render_path::update_statistics_internal()
    {
        auto application_p = F_application::instance_p();

        statistics_times_.fps += application_p->delta_seconds();
        if(statistics_times_.fps >= statistics_durations_.fps)
        {
            statistics_.fps = application_p->fps();
            statistics_times_.fps = 0.0f;
        }

        statistics_times_.frame_time += application_p->delta_seconds();
        if(statistics_times_.frame_time >= statistics_durations_.frame_time)
        {
            statistics_.frame_time = application_p->delta_seconds() * 1000.0f;
            statistics_times_.frame_time = 0.0f;
        }

        frame_data_.delta_time = application_p->delta_seconds();
        frame_data_.time += frame_data_.delta_time;
        frame_data_.sin_time = sin(frame_data_.time);
        frame_data_.cos_time = cos(frame_data_.time);
    }

    void F_render_path::RG_init_register()
    {
        A_example::instance_p()->RG_init_register();
    }
    void F_render_path::RG_early_register()
    {
        if (is_first_frame_)
        {
            RG_init_register();
            is_first_frame_ = false;
        }
        
        newrg::F_render_path::RG_early_register();

        if(!enable)
            return;

        A_example::instance_p()->RG_early_register();
    }
    void F_render_path::RG_begin_register()
    {
        newrg::F_render_path::RG_begin_register();

        update_statistics_internal();

        if(!enable)
            return;

        newrg::H_scene_render_view::RG_begin_register_all();

        A_example::instance_p()->RG_begin_register();
    }
    void F_render_path::RG_end_register()
    {
        newrg::F_render_path::RG_end_register();

        if(!enable)
            return;

        newrg::H_scene_render_view::for_each(
            [this](TKPA_valid<newrg::F_scene_render_view> view_p)
            {
                TK<F_scene_render_view> casted_view_p;
                if(view_p.T_try_interface<F_scene_render_view>(casted_view_p))
                {
                    auto render_graph_p = newrg::F_render_graph::instance_p();

                    auto view_size = casted_view_p->size();

                    //
                    A_example::instance_p()->RG_register_view(NCPP_FOH_VALID(casted_view_p));
                }
            }
        );

        A_example::instance_p()->RG_end_register();

        newrg::H_scene_render_view::RG_end_register_all();
    }

    void F_render_path::update_ui()
    {
        auto begin_section = [](auto&& name)
        {
            ImGui::BeginChild(name, ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            ImGui::Text(name);
        };
        auto end_section = []()
        {
            ImGui::EndChild();
            ImGui::Dummy(ImVec2(0, 10));
        };

        // Options
        {
            ImGui::Begin("DXRT Render Path Options");

            ImGui::Checkbox("Enable", &enable);

            ImGui::End();
        }

        // Statistics
        {
            ImGui::Begin("DXRT Render Path Statistics");

            ImGui::Text("FPS: %.2f", statistics_.fps);
            ImGui::InputFloat("FPS Update Duration", &statistics_durations_.fps);

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::Text("Frame Time (ms): %.4f", statistics_.frame_time);
            ImGui::InputFloat("Frame Time Update Duration", &statistics_durations_.frame_time);

            ImGui::End();
        }
    }
}
