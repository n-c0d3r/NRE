#pragma once

#include "dxrt_prerequisites.hpp"

namespace dxrt
{
    class F_scene_render_view;

    class NRE_API F_scene_render_view :
        public newrg::F_scene_render_view
    {
    private:
        b8 is_register_ = true;

    public:
        F_vector3 clear_color = F_vector3::zero();
        ED_format color_format = ED_format::R8G8B8A8_UNORM;

    public:

    public:
        F_scene_render_view(TKPA_valid<F_actor> actor_p, A_render_view_mask mask = 0);
        virtual ~F_scene_render_view();

    public:
        NCPP_OBJECT(F_scene_render_view);

    protected:
        virtual void update_ui();

    protected:
        virtual void gameplay_tick() override;
        virtual void render_tick() override;
        
    public:
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;
    };
    
    class F_render_factory_proxy final : public newrg::F_delegable_render_factory_proxy
    {
    public:
        F_render_factory_proxy();
        ~F_render_factory_proxy();

    public:
        NCPP_OBJECT(F_render_factory_proxy);

    public:
        virtual TK_valid<A_render_view> create_scene_render_view(TKPA_valid<F_actor> actor_p) override;
    };

    class F_render_path : public newrg::F_render_path
    {
    private:
        struct F_statistics
        {
            f32 fps = 0.0f;
            f32 frame_time = 0.0f;
        };
        F_statistics statistics_;

        struct F_statistics_times
        {
            f32 fps = 0.0f;
            f32 frame_time = 0.0f;
        };
        F_statistics_times statistics_times_;

        struct F_statistics_durations
        {
            f32 fps = 1.0f;
            f32 frame_time = 1.0f;
        };
        F_statistics_durations statistics_durations_;

        b8 is_first_frame_ = true;
        
    public:
        struct NCPP_ALIGN(16) F_frame_data
        {
            f32 time = 0.0f;
            f32 delta_time;
            f32 sin_time;
            f32 cos_time;
        };

        u32 min_wave_size_ = 0;
        u32 max_wave_size_ = 0;
        u32 total_lane_count_ = 0;

        b8 enable = true;

    private:
        F_frame_data frame_data_;

    public:
        NCPP_FORCE_INLINE const auto& statistics() const noexcept { return statistics_; }
        NCPP_FORCE_INLINE const auto& statistics_times() const noexcept { return statistics_times_; }
        NCPP_FORCE_INLINE const auto& statistics_durations() const noexcept { return statistics_durations_; }

        NCPP_FORCE_INLINE const auto& frame_data() const noexcept { return frame_data_; }



    public:
        F_render_path();
        virtual ~F_render_path();

    public:
        NCPP_OBJECT(F_render_path);

    private:
        void update_statistics_internal();

    public:
        virtual void RG_init_register();
        virtual void RG_early_register() override;
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;

    public:
        virtual void update_ui();
    };
}



#define DXRT_RENDER_PATH() NRE_NEWRG_RENDER_PATH().T_cast<dxrt::F_render_path>()