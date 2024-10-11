#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_large_data_list.hpp>
#include <nre/rendering/newrg/render_pass_utilities.hpp>
#include <nre/utilities/large_data_list.hpp>



namespace nre::newrg
{
    template<typename F_element__>
    class NRE_API TF_gpu_subpage_based_uploader final
    {
    public:
        using F_element = F_element__;
        using F_gpu_large_data_list = TF_gpu_large_data_list<F_element>;
        using F_cpu_large_data_list = TF_large_data_list<F_element>;



    private:
        F_gpu_large_data_list* gpu_large_data_list_p_ = 0;
        F_cpu_large_data_list* cpu_large_data_list_p_ = 0;
        u32 subpage_count_per_page_ = 1;

        TG_vector<b8> subpage_id_to_need_to_upload_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE F_gpu_large_data_list* gpu_large_data_list_p() const noexcept { return gpu_large_data_list_p_; }
        NCPP_FORCE_INLINE F_cpu_large_data_list* cpu_large_data_list_p() const noexcept { return cpu_large_data_list_p_; }
        NCPP_FORCE_INLINE u32 subpage_count_per_page() const noexcept { return subpage_count_per_page_; }

        NCPP_FORCE_INLINE const auto& subpage_id_to_need_to_upload() const noexcept { return subpage_id_to_need_to_upload_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; });



    public:
        TF_gpu_subpage_based_uploader() = default;
        TF_gpu_subpage_based_uploader(
            F_gpu_large_data_list* gpu_large_data_list_p,
            F_cpu_large_data_list* cpu_large_data_list_p,
            u32 subpage_count_per_page = 1
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        ) :
            gpu_large_data_list_p_(gpu_large_data_list_p),
            cpu_large_data_list_p_(cpu_large_data_list_p),
            subpage_count_per_page_(subpage_count_per_page)
            NRE_OPTIONAL_DEBUG_PARAM(name_(name))
        {
            NCPP_ASSERT(gpu_large_data_list_p);
            NCPP_ASSERT(cpu_large_data_list_p);
            NCPP_ASSERT(subpage_count_per_page);
            NCPP_ASSERT(
                (
                    gpu_large_data_list_p->page_capacity_in_elements()
                    % subpage_count_per_page
                )
                == 0
            );
        }
        ~TF_gpu_subpage_based_uploader()
        {
            reset();
        }

        TF_gpu_subpage_based_uploader(const TF_gpu_subpage_based_uploader& x) :
            gpu_large_data_list_p_(x.gpu_large_data_list_p_),
            cpu_large_data_list_p_(x.cpu_large_data_list_p_),
            subpage_count_per_page_(x.subpage_count_per_page_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);
        }
        TF_gpu_subpage_based_uploader& operator = (const TF_gpu_subpage_based_uploader& x)
        {
            gpu_large_data_list_p_ = x.gpu_large_data_list_p_;
            cpu_large_data_list_p_ = x.cpu_large_data_list_p_;
            subpage_count_per_page_ = x.subpage_count_per_page_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            return *this;
        }

        TF_gpu_subpage_based_uploader(TF_gpu_subpage_based_uploader&& x) :
            gpu_large_data_list_p_(x.gpu_large_data_list_p_),
            cpu_large_data_list_p_(x.cpu_large_data_list_p_),
            subpage_count_per_page_(x.subpage_count_per_page_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();
        }
        TF_gpu_subpage_based_uploader& operator = (TF_gpu_subpage_based_uploader&& x)
        {
            gpu_large_data_list_p_ = x.gpu_large_data_list_p_;
            cpu_large_data_list_p_ = x.cpu_large_data_list_p_;
            subpage_count_per_page_ = x.subpage_count_per_page_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();

            return *this;
        }

    public:
        void reset()
        {
            gpu_large_data_list_p_ = 0;
            cpu_large_data_list_p_ = 0;
            subpage_count_per_page_ = 0;
        }

    public:
        void RG_begin_register()
        {
            NCPP_ASSERT(gpu_large_data_list_p_->usuable_page_count() == cpu_large_data_list_p_->usuable_page_count());
            NCPP_ASSERT(gpu_large_data_list_p_->page_capacity_in_elements() == cpu_large_data_list_p_->page_capacity_in_elements());

            sz usuable_page_count = gpu_large_data_list_p_->usuable_page_count();
            subpage_id_to_need_to_upload_.resize(usuable_page_count * subpage_count_per_page_);
            for(auto& need_to_upload : subpage_id_to_need_to_upload_)
            {
                need_to_upload = false;
            }
        }
        void RG_end_register()
        {
            auto& rg_page_p_vector = gpu_large_data_list_p_->rg_page_p_vector();

            auto page_capacity = sizeof(F_element__) * gpu_large_data_list_p_->page_capacity_in_elements();
            auto subpage_capacity = page_capacity / subpage_count_per_page_;

            u32 last_page_index = NCPP_U32_MAX;
            sz last_local_page_offset = 0;
            u8* last_data_begin_p = 0;
            u8* last_data_end_p = 0;

            auto try_register_upload_data = [&]()
            {
                if(last_page_index != NCPP_U32_MAX)
                {
                    H_render_pass::upload_buffer(
                        rg_page_p_vector[last_page_index],
                        last_local_page_offset,
                        { last_data_begin_p, sz(last_data_end_p - last_data_begin_p) }
                        NRE_OPTIONAL_DEBUG_PARAM(
                            (
                                name_
                                + ".upload_page_passes["
                                + G_to_string(last_page_index).c_str()
                                + "] (system memory: "
                                + decimal_to_hex(sz(last_data_begin_p)).c_str()
                                + " -> "
                                + decimal_to_hex(sz(last_data_end_p)).c_str()
                                + ")"
                            ).c_str()
                        )
                    );
                }

                last_page_index = NCPP_U32_MAX;
                last_local_page_offset = 0;
                last_data_begin_p = 0;
                last_data_end_p = 0;
            };

            sz subpage_count = subpage_id_to_need_to_upload_.size();
            for(sz subpage_index = 0; subpage_index < subpage_count; ++subpage_index)
            {
                if(subpage_id_to_need_to_upload_[subpage_index])
                {
                    auto page_index = subpage_index / subpage_count_per_page_;

                    auto rg_page_p = rg_page_p_vector[page_index];
                    auto& src_data_page = cpu_large_data_list_p_->page(page_index);

                    auto local_page_offset = subpage_capacity * (subpage_index % subpage_count_per_page_);

                    u8* data_begin_p = (u8*)(src_data_page.data()) + local_page_offset;
                    u8* data_end_p = data_begin_p + subpage_capacity;

                    if(
                        (data_begin_p == last_data_end_p)
                        || (last_data_begin_p == 0)
                    )
                    {
                        if(last_data_begin_p == 0)
                        {
                            last_page_index = page_index;
                            last_local_page_offset = local_page_offset;
                            last_data_begin_p = data_begin_p;
                        }
                        last_data_end_p = data_end_p;
                    }
                    else
                    {
                        try_register_upload_data();

                        last_page_index = page_index;
                        last_local_page_offset = local_page_offset;
                        last_data_begin_p = data_begin_p;
                        last_data_end_p = data_end_p;
                    }
                }
            }

            try_register_upload_data();
        }

    public:
        void enqueue_upload(u32 subpage_index)
        {
            subpage_id_to_need_to_upload_[subpage_index] = true;
        }
        void enqueue_upload(u32 page_index, u32 local_subpage_index)
        {
            subpage_id_to_need_to_upload_[page_index * subpage_count_per_page_ + local_subpage_index] = true;
        }
    };
}