#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_large_data_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/utilities/large_data_list.hpp>



namespace nre::newrg
{
    template<typename F_element__>
    class NRE_API TF_gpu_page_based_uploader final
    {
    public:
        using F_element = F_element__;
        using F_gpu_large_data_list = TF_gpu_large_data_list<F_element>;
        using F_cpu_large_data_list = TF_large_data_list<F_element>;



    private:
        F_gpu_large_data_list* gpu_large_data_list_p_ = 0;
        F_cpu_large_data_list* cpu_large_data_list_p_ = 0;

        TG_vector<b8> page_id_to_need_to_upload_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE F_gpu_large_data_list* gpu_large_data_list_p() const noexcept { return gpu_large_data_list_p_; }
        NCPP_FORCE_INLINE F_cpu_large_data_list* cpu_large_data_list_p() const noexcept { return cpu_large_data_list_p_; }

        NCPP_FORCE_INLINE const auto& page_id_to_need_to_upload() const noexcept { return page_id_to_need_to_upload_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; });



    public:
        TF_gpu_page_based_uploader() = default;
        TF_gpu_page_based_uploader(
            F_gpu_large_data_list* gpu_large_data_list_p,
            F_cpu_large_data_list* cpu_large_data_list_p
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        ) :
            gpu_large_data_list_p_(gpu_large_data_list_p),
            cpu_large_data_list_p_(cpu_large_data_list_p)
            NRE_OPTIONAL_DEBUG_PARAM(name_(name))
        {
        }
        ~TF_gpu_page_based_uploader()
        {
            reset();
        }

        TF_gpu_page_based_uploader(const TF_gpu_page_based_uploader& x) :
            gpu_large_data_list_p_(x.gpu_large_data_list_p_),
            cpu_large_data_list_p_(x.cpu_large_data_list_p_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);
        }
        TF_gpu_page_based_uploader& operator = (const TF_gpu_page_based_uploader& x)
        {
            gpu_large_data_list_p_ = x.gpu_large_data_list_p_;
            cpu_large_data_list_p_ = x.cpu_large_data_list_p_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            return *this;
        }

        TF_gpu_page_based_uploader(TF_gpu_page_based_uploader&& x) :
            gpu_large_data_list_p_(x.gpu_large_data_list_p_),
            cpu_large_data_list_p_(x.cpu_large_data_list_p_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();
        }
        TF_gpu_page_based_uploader& operator = (TF_gpu_page_based_uploader&& x)
        {
            gpu_large_data_list_p_ = x.gpu_large_data_list_p_;
            cpu_large_data_list_p_ = x.cpu_large_data_list_p_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();

            return *this;
        }

    public:
        void reset()
        {
            gpu_large_data_list_p_ = 0;
            cpu_large_data_list_p_ = 0;
        }

    public:
        void RG_begin_register()
        {
            NCPP_ASSERT(gpu_large_data_list_p_->element_count() == cpu_large_data_list_p_->element_count());
            NCPP_ASSERT(gpu_large_data_list_p_->page_capacity_in_elements() == cpu_large_data_list_p_->page_capacity_in_elements());

            sz usuable_page_count = gpu_large_data_list_p_->usuable_page_count();
            page_id_to_need_to_upload_.resize(usuable_page_count);
            for(auto& need_to_upload : page_id_to_need_to_upload_)
            {
                need_to_upload = false;
            }
        }
        void RG_end_register()
        {
            auto& rg_page_p_vector = gpu_large_data_list_p_->rg_page_p_vector();

            sz usuable_page_count = page_id_to_need_to_upload_.size();
            for(sz page_index = 0; page_index < usuable_page_count; ++page_index)
            {
                if(page_id_to_need_to_upload_[page_index])
                {
                    auto rg_page_p = rg_page_p_vector[page_index];
                    auto& src_data_page = cpu_large_data_list_p_->page(page_index);

                    TG_span<u8> data_span = { (u8*)(src_data_page.data()), sizeof(F_element__) * gpu_large_data_list_p_->page_capacity_in_elements() };

                    H_gpu_render_pass::upload_buffer(
                        rg_page_p,
                        0,
                        data_span
                        NRE_OPTIONAL_DEBUG_PARAM((name_ + ".upload_page_passes[" + G_to_string(page_index).c_str() + "]").c_str())
                    );
                }
            }
        }

    public:
        void enqueue_upload(u32 page_index)
        {
            page_id_to_need_to_upload_[page_index] = true;
        }
    };
}