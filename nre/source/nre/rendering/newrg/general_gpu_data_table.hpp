#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/gpu_large_data_list.hpp>
#include <nre/rendering/newrg/gpu_range_based_uploader.hpp>
#include <nre/utilities/general_data_distributor.hpp>



namespace nre::newrg
{
    class F_render_resource;



    template<typename... Fs_element__>
    class NRE_API TF_general_gpu_data_table final
    {
    public:
        using F_element_targ_list = TF_template_targ_list<Fs_element__...>;
        static constexpr sz row_count = sizeof...(Fs_element__);
        NCPP_STATIC_ASSERT(row_count > 0, "empty registry is not allowed");

        using F_gpu_large_data_list_tuple = TG_tuple<TF_gpu_large_data_list<Fs_element__>...>;
        using F_gpu_range_based_uploader_tuple = TG_tuple<TF_gpu_range_based_uploader<Fs_element__>...>;



    private:
        F_general_data_distributor general_data_distributor_;

        F_gpu_large_data_list_tuple gpu_large_data_list_tuple_;
        F_gpu_range_based_uploader_tuple gpu_range_based_uploader_tuple_;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE const auto& general_data_distributor() const noexcept { return general_data_distributor_; }
        NCPP_FORCE_INLINE const auto& gpu_large_data_list_tuple() const noexcept { return gpu_large_data_list_tuple_; }
        NCPP_FORCE_INLINE const auto& gpu_range_based_uploader_tuple() const noexcept { return gpu_range_based_uploader_tuple_; }

        template<sz row_index__ = 0>
        NCPP_FORCE_INLINE const auto& T_gpu_large_data_list_tuple() const noexcept { return eastl::get<row_index__>(gpu_large_data_list_tuple_); }
        template<sz row_index__ = 0>
        NCPP_FORCE_INLINE const auto& T_gpu_range_based_uploader_tuple() const noexcept { return eastl::get<row_index__>(gpu_range_based_uploader_tuple_); }



    public:
        TF_general_gpu_data_table() = default;
        TF_general_gpu_data_table(
            ED_resource_flag flags,
            ED_resource_heap_type heap_type,
            ED_resource_state initial_state,
            sz page_capacity_in_elements = 1024,
            sz page_count = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        ) :
            general_data_distributor_(page_capacity_in_elements)
            NRE_OPTIONAL_DEBUG_PARAM(name_(name))
        {
            T_setup_row<0>(
                flags,
                heap_type,
                initial_state,
                page_capacity_in_elements,
                page_count
                NRE_OPTIONAL_DEBUG_PARAM(name)
            );
            T_link_data_lists_to_uploader<0>();
        }
        ~TF_general_gpu_data_table()
        {
            reset();
        }

        TF_general_gpu_data_table(const TF_general_gpu_data_table& x) :
            general_data_distributor_(x.general_data_distributor_),
            gpu_large_data_list_tuple_(x.gpu_large_data_list_tuple_)
        {
            T_link_data_lists_to_uploader<0>();
        }
        TF_general_gpu_data_table& operator = (const TF_general_gpu_data_table& x)
        {
            general_data_distributor_ = x.general_data_distributor_;
            gpu_large_data_list_tuple_ = x.gpu_large_data_list_tuple_;

            T_link_data_lists_to_uploader<0>();

            return *this;
        }

        TF_general_gpu_data_table(TF_general_gpu_data_table&& x) :
            general_data_distributor_(eastl::move(x.general_data_distributor_)),
            gpu_large_data_list_tuple_(eastl::move(x.gpu_large_data_list_tuple_))
        {
            T_link_data_lists_to_uploader<0>();

            x.reset();
        }
        TF_general_gpu_data_table& operator = (TF_general_gpu_data_table&& x)
        {
            general_data_distributor_ = eastl::move(x.general_data_distributor_);
            gpu_large_data_list_tuple_ = eastl::move(x.gpu_large_data_list_tuple_);

            T_link_data_lists_to_uploader<0>();

            x.reset();

            return *this;
        }



    private:
        template<sz row_index__>
        void T_setup_row(
            ED_resource_flag flags,
            ED_resource_heap_type heap_type,
            ED_resource_state initial_state,
            sz page_capacity_in_elements = 1024,
            sz page_count = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        )
        {
            if constexpr ((row_index__ + 1) < row_count)
            {
                T_setup_row<(row_index__ + 1 < row_index__) ? (row_index__ + 1) : 0>(
                    flags,
                    heap_type,
                    initial_state,
                    page_capacity_in_elements,
                    page_count,
                    name
                );
            }

            using F_element = typename F_element_targ_list::template TF_at<row_index__>;

            eastl::get<row_index__>(gpu_large_data_list_tuple_) = TF_gpu_large_data_list<F_element>(
                flags,
                heap_type,
                initial_state,
                0,
                page_capacity_in_elements,
                page_count
                NRE_OPTIONAL_DEBUG_PARAM(name + ".gpu_large_data_lists[" + T_type_fullname<F_element>() + "]")
            );
        }
        template<sz row_index__>
        void T_reset_row()
        {
            if constexpr ((row_index__ + 1) < row_count)
            {
                T_reset_row<(row_index__ + 1 < row_index__) ? (row_index__ + 1) : 0>();
            }

            eastl::get<row_index__>(gpu_large_data_list_tuple_).reset();
            eastl::get<row_index__>(gpu_range_based_uploader_tuple_).reset();
        }
        template<sz row_index__>
        void T_link_data_lists_to_uploader()
        {
            if constexpr ((row_index__ + 1) < row_count)
            {
                T_link_data_lists_to_uploader<(row_index__ + 1 < row_index__) ? (row_index__ + 1) : 0>();
            }

            using F_element = typename F_element_targ_list::template TF_at<row_index__>;

            eastl::get<row_index__>(gpu_range_based_uploader_tuple_) = TF_gpu_range_based_uploader<F_element>(
                &eastl::get<row_index__>(gpu_large_data_list_tuple_)
                NRE_OPTIONAL_DEBUG_PARAM(name_ + ".gpu_range_based_uploaders[" + T_type_fullname<F_element>() + "]")
            );
        }
        template<sz row_index__>
        void TRG_end_register()
        {
            if constexpr ((row_index__ + 1) < row_count)
            {
                TRG_end_register<(row_index__ + 1 < row_index__) ? (row_index__ + 1) : 0>();
            }

            eastl::get<row_index__>(gpu_large_data_list_tuple_).RG_begin_register();
            eastl::get<row_index__>(gpu_large_data_list_tuple_).set_element_count(general_data_distributor_.capacity());
            eastl::get<row_index__>(gpu_large_data_list_tuple_).RG_end_register();
        }



    public:
        void reset()
        {
            general_data_distributor_.reset();
            T_reset_row<0>();
        }

    public:
        void RG_begin_register()
        {
        }
        void RG_end_register()
        {
            TRG_end_register<0>();
        }

    public:
        /**
         *  Non-thread-safe
         */
        NCPP_FORCE_INLINE sz register_id(sz count = 1, sz alignment = 1)
        {
            return general_data_distributor_.register_id(count, alignment);
        }
        /**
         *  Non-thread-safe
         */
        NCPP_FORCE_INLINE void deregister_id(sz id)
        {
            general_data_distributor_.deregister_id(id);
        }

    public:
        /**
         *  Thread-safe
         */
        NCPP_FORCE_INLINE sz capacity() const noexcept
        {
            return general_data_distributor_.capacity();
        }

    public:
        /**
         *  Thread-safe
         */
        template<sz row_index__ = 0>
        NCPP_FORCE_INLINE void T_upload(sz begin_id, const TG_span<typename F_element_targ_list::template TF_at<row_index__>>& data)
        {
            eastl::get<row_index__>(gpu_range_based_uploader_tuple_).upload(
                begin_id,
                data
            );
        }
    };
}