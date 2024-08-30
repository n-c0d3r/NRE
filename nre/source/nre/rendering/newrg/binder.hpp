#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_pass_flag.hpp>



namespace nre::newrg
{
    class A_binder
    {
    private:
        TK_valid<A_command_list> command_list_p_;
        TK_valid<A_pipeline_state> pipeline_state_p_;

    public:
        NCPP_FORCE_INLINE TKPA_valid<A_command_list> command_list_p() const noexcept { return command_list_p_; }
        NCPP_FORCE_INLINE TKPA_valid<A_pipeline_state> pipeline_state_p() const noexcept { return pipeline_state_p_; }



    protected:
        NCPP_FORCE_INLINE A_binder(
            TKPA_valid<A_command_list> command_list_p,
            TKPA_valid<A_pipeline_state> pipeline_state_p
        ) noexcept :
            command_list_p_(command_list_p),
            pipeline_state_p_(pipeline_state_p)
        {
            command_list_p_->bind_pipeline_state(pipeline_state_p);
        }
        NCPP_FORCE_INLINE A_binder(
            TKPA_valid<A_command_list> command_list_p,
            KPA_valid_graphics_pipeline_state_handle pipeline_state_p
        ) noexcept :
            command_list_p_(command_list_p),
            pipeline_state_p_(pipeline_state_p.oref)
        {
            command_list_p_->ZG_bind_pipeline_state(pipeline_state_p);
        }
        NCPP_FORCE_INLINE A_binder(
            TKPA_valid<A_command_list> command_list_p,
            KPA_valid_compute_pipeline_state_handle pipeline_state_p
        ) noexcept :
            command_list_p_(command_list_p),
            pipeline_state_p_(pipeline_state_p.oref)
        {
            command_list_p_->ZC_bind_pipeline_state(pipeline_state_p);
        }

    public:
        NCPP_FORCE_INLINE ~A_binder() noexcept {}

    public:
        A_binder(const A_binder&) = delete;
        A_binder& operator = (const A_binder&) = delete;
        A_binder(A_binder&&) = delete;
        A_binder& operator = (A_binder&&) = delete;
    };
}