#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class F_transient_resource_uploader;
    class F_cpu_gpu_data_pool_page;
    class F_cpu_gpu_data_pool;
    class F_render_resource;



    class NRE_API F_cpu_gpu_data_pool_page final
    {
    public:
        friend class F_cpu_gpu_data_pool;



    private:
        u32 element_stride_ = 0;
        u32 element_alignment_ = 0;
        u32 capacity_in_elements_ = 0;

        ab8 need_to_upload_ = false;

        au32 obtained_element_count_ = 0;
        TF_concurrent_ring_buffer<u32> local_element_index_caches_;

        TG_vector<u8> data_;

    public:
        NCPP_FORCE_INLINE u32 element_stride() const noexcept { return element_stride_; }
        NCPP_FORCE_INLINE u32 element_algnment_() const noexcept { return element_alignment_; }
        NCPP_FORCE_INLINE u32 capacity_in_elements() const noexcept { return capacity_in_elements_; }

        NCPP_FORCE_INLINE u32 obtained_element_count() const noexcept { return obtained_element_count_.load(eastl::memory_order_acquire); }
        NCPP_FORCE_INLINE const auto& local_element_index_caches() const noexcept { return local_element_index_caches_; }

        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }



    public:
        F_cpu_gpu_data_pool_page() = default;
        F_cpu_gpu_data_pool_page(
            u32 element_stride,
            u32 element_alignment,
            u32 capacity_in_elements,
            u32 obtained_element_count
        );
        ~F_cpu_gpu_data_pool_page();

        F_cpu_gpu_data_pool_page(F_cpu_gpu_data_pool_page&& x);
        F_cpu_gpu_data_pool_page& operator = (F_cpu_gpu_data_pool_page&& x);



    public:
        u32 try_register_data();
        void deregister_data(u32 local_index);

    public:
        void register_upload(u32 local_index, void* src_data_p);
    };



    class NRE_API F_cpu_gpu_data_pool
    {
    private:
        ED_resource_flag resource_flags_ = ED_resource_flag::NONE;
        u32 element_stride_ = 0;
        u32 element_alignment_ = 0;
        u32 page_capacity_in_elements_ = 0;
        au32 current_capacity_in_elements_ = 0;

        TG_vector<F_cpu_gpu_data_pool_page> pages_;

        U_buffer_handle target_buffer_p_;
        F_render_resource* rg_target_buffer_p_ = 0;

        U_buffer_handle src_resize_buffer_p_;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_started_register_data_ = false);
        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_started_register_upload_ = false);

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE ED_resource_flag resource_flags() const noexcept { return resource_flags_; }
        NCPP_FORCE_INLINE u32 element_stride() const noexcept { return element_stride_; }
        NCPP_FORCE_INLINE u32 element_alignment() const noexcept { return element_alignment_; }
        NCPP_FORCE_INLINE u32 page_capacity_in_elements() const noexcept { return page_capacity_in_elements_; }
        NCPP_FORCE_INLINE u32 current_capacity_in_elements() const noexcept { return current_capacity_in_elements_.load(eastl::memory_order_acquire); }

        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }

        NCPP_FORCE_INLINE auto target_buffer_p() const noexcept { return NCPP_FOH_VALID(target_buffer_p_); }
        NCPP_FORCE_INLINE auto rg_target_buffer_p() const noexcept { return rg_target_buffer_p_; }

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
            NCPP_FORCE_INLINE const auto& name() const noexcept { return name_; }
        );

        NCPP_FORCE_INLINE u32 page_index(u32 element_index) const noexcept
        {
            u32 result = element_index / page_capacity_in_elements_;

            NCPP_ASSERT(result < pages_.size());

            return result;
        }



    public:
        F_cpu_gpu_data_pool(
            ED_resource_flag resource_flags,
            u32 element_stride,
            u32 element_alignment,
            u32 page_capacity_in_elements
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        );
        virtual ~F_cpu_gpu_data_pool();

    public:
        NCPP_OBJECT(F_cpu_gpu_data_pool);



    private:
        void register_rg_target_buffer_internal();



    public:
        virtual void RG_begin_register_data();
        virtual void RG_end_register_data();

    public:
        virtual void RG_begin_register_upload();
        virtual void RG_end_register_upload();

    public:
        u32 register_data();
        void deregister_data(u32 index);

    public:
        void register_upload(u32 index, void* src_data_p);

    public:
        const void* data_p(u32 index) const;
        void* data_p(u32 index);
    };



    template<typename F_element__>
    class NRE_API TF_cpu_gpu_data_pool : public F_cpu_gpu_data_pool
    {
    public:
        using F_element = F_element__;



    public:
        TF_cpu_gpu_data_pool(
            ED_resource_flag resource_flags,
            u32 page_capacity_in_elements
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        ) :
            F_cpu_gpu_data_pool(
                resource_flags,
                sizeof(F_element__),
                NCPP_ALIGNOF(F_element__),
                page_capacity_in_elements
                NRE_OPTIONAL_DEBUG_PARAM(name)
            )
        {}

    public:
        NCPP_FORCE_INLINE F_element__& data(u32 index)
        {
            *(F_element__*)data_p(index);
        }
        NCPP_FORCE_INLINE const F_element__& data(u32 index) const
        {
            *(const F_element__*)data_p(index);
        }
    };
}