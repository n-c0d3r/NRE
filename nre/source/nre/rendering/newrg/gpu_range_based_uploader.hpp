#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_large_data_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/utilities/large_data_list.hpp>



namespace nre::newrg
{
    template<typename F_element__>
    class NRE_API TF_gpu_range_based_uploader final
    {
    public:
        using F_element = F_element__;
        using F_gpu_large_data_list = TF_gpu_large_data_list<F_element>;



    private:
        F_gpu_large_data_list* gpu_large_data_list_p_ = 0;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE F_gpu_large_data_list* gpu_large_data_list_p() const noexcept { return gpu_large_data_list_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; });



    public:
        TF_gpu_range_based_uploader() = default;
        TF_gpu_range_based_uploader(
            F_gpu_large_data_list* gpu_large_data_list_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        ) :
            gpu_large_data_list_p_(gpu_large_data_list_p)
            NRE_OPTIONAL_DEBUG_PARAM(name_(name))
        {
        }
        ~TF_gpu_range_based_uploader()
        {
            reset();
        }

        TF_gpu_range_based_uploader(const TF_gpu_range_based_uploader& x) :
            gpu_large_data_list_p_(x.gpu_large_data_list_p_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);
        }
        TF_gpu_range_based_uploader& operator = (const TF_gpu_range_based_uploader& x)
        {
            gpu_large_data_list_p_ = x.gpu_large_data_list_p_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            return *this;
        }

        TF_gpu_range_based_uploader(TF_gpu_range_based_uploader&& x) :
            gpu_large_data_list_p_(x.gpu_large_data_list_p_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();
        }
        TF_gpu_range_based_uploader& operator = (TF_gpu_range_based_uploader&& x)
        {
            gpu_large_data_list_p_ = x.gpu_large_data_list_p_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();

            return *this;
        }

    public:
        void reset()
        {
            gpu_large_data_list_p_ = 0;
        }

    public:
        void upload(sz begin_element_index, TG_span<F_element> data)
        {
            sz page_index = gpu_large_data_list_p_->page_index(begin_element_index);
            sz local_element_index = gpu_large_data_list_p_->local_element_index(begin_element_index);

            H_gpu_render_pass::upload_buffer(
                gpu_large_data_list_p_->rg_page_p(page_index),
                local_element_index * sizeof(F_element),
                { (u8*)data.data(), data.size() * sizeof(F_element) }
                NRE_OPTIONAL_DEBUG_PARAM((name_ + ".upload_page_passes[" + G_to_string(page_index).c_str() + "]").c_str())
            );
        }
    };
}