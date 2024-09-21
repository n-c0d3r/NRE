#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_bind_list.hpp>
#include <nre/rendering/newrg/general_gpu_data_table.hpp>
#include <nre/rendering/newrg/cacheable_pool_gpu_data_table.hpp>
#include <nre/rendering/newrg/uncacheable_pool_gpu_data_table.hpp>



namespace nre::newrg
{
    template<template<typename... Fs_element_t__> class TF_table__, typename... Fs_element__>
    class NRE_API TF_gpu_data_table_render_bind_list final
    {
    public:
        using F_element_targ_list = TF_template_targ_list<Fs_element__...>;
        static constexpr sz row_count = sizeof...(Fs_element__);
        NCPP_STATIC_ASSERT(row_count > 0, "empty registry is not allowed");

        template<typename... Fs_element_in__>
        using TF_table = TF_table__<Fs_element_in__...>;
        using F_table = TF_table__<Fs_element__...>;



    private:
        TG_array<F_render_bind_list, row_count> bases_;
        F_table* table_p_ = 0;

        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_render_frame_name name_);

    public:
        NCPP_FORCE_INLINE const auto& bases() const noexcept { return bases_; }
        NCPP_FORCE_INLINE auto& bases() noexcept { return bases_; }
        NCPP_FORCE_INLINE const F_table* table_p() const noexcept { return table_p_; }
        NCPP_FORCE_INLINE F_table* table_p() noexcept { return table_p_; }



    public:
        TF_gpu_data_table_render_bind_list() = default;
        TF_gpu_data_table_render_bind_list(
            F_table* table_p,
            const TG_array<ED_resource_view_type, row_count>& types,
            const TG_array<ED_resource_flag, row_count>& overrided_resource_flags = (
                []()
                {
                    TG_array<ED_resource_flag, row_count> result;
                    for(sz i = 0; i < row_count; ++i)
                        result[i] = ED_resource_flag::NONE;
                    return result;
                }
            )(),
            const TG_array<ED_format, row_count>& overrided_formats = (
                []()
                {
                    TG_array<ED_format, row_count> result;
                    for(sz i = 0; i < row_count; ++i)
                        result[i] = ED_format::NONE;
                    return result;
                }
            )()
            NRE_OPTIONAL_DEBUG_PARAM(const F_render_frame_name& name = "")
        ) :
            table_p_(table_p)
            NRE_OPTIONAL_DEBUG_PARAM(name_(name))
        {
            T_setup_base<0>(
                types,
                overrided_resource_flags,
                overrided_formats
            );
        }
        ~TF_gpu_data_table_render_bind_list()
        {
            reset();
        }

        TF_gpu_data_table_render_bind_list(TF_gpu_data_table_render_bind_list&& x) :
            bases_(eastl::move(bases_)),
            table_p_(x.table_p_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();
        }
        TF_gpu_data_table_render_bind_list& operator = (TF_gpu_data_table_render_bind_list&& x)
        {
            bases_ = eastl::move(x.bases_);
            table_p_ = x.table_p_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();

            return *this;
        }



    private:
        template<sz row_index__>
        void T_setup_base(
            const TG_array<ED_resource_view_type, row_count>& types,
            const TG_array<ED_resource_flag, row_count>& overrided_resource_flags,
            const TG_array<ED_format, row_count>& overrided_formats
        )
        {
            if constexpr ((row_index__ + 1) < row_count)
            {
                T_setup_base<(row_index__ + 1 < row_count) ? (row_index__ + 1) : 0>(
                    types,
                    overrided_resource_flags,
                    overrided_formats
                );
            }

            sz usuable_page_count = table_p_->usuable_page_count();

            bases_[row_index__] = F_render_bind_list(
                ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS,
                table_p_->usuable_page_count()
                NRE_OPTIONAL_DEBUG_PARAM(name_ + "[" + T_type_fullname<typename F_element_targ_list::template TF_at<row_index__>>() + "]")
            );

            auto& gpu_large_data_list = table_p_->T_gpu_large_data_list_tuple<row_index__>();
            auto& rg_page_p_vector = gpu_large_data_list.rg_page_p_vector();

            for(sz i = 0; i < usuable_page_count; ++i)
            {
                bases_[row_index__].enqueue_initialize_resource_view(
                    rg_page_p_vector[i],
                    {
                        .type = types[row_index__],
                        .overrided_resource_flags = overrided_resource_flags[row_index__],
                        .overrided_format = overrided_formats[row_index__]
                    },
                    i
                );
            }
        }



    public:
        void reset()
        {
            for(sz i = 0; i < row_count; ++i)
                bases_[i].reset();

            table_p_ = 0;
        }
    };



    template<typename... Fs_element__>
    using TF_general_gpu_data_table_render_bind_list = TF_gpu_data_table_render_bind_list<
        TF_general_gpu_data_table,
        Fs_element__...
    >;
    template<typename... Fs_element__>
    using TF_cacheable_pool_gpu_data_table_render_bind_list = TF_gpu_data_table_render_bind_list<
        TF_cacheable_pool_gpu_data_table,
        Fs_element__...
    >;
    template<typename... Fs_element__>
    using TF_uncacheable_pool_gpu_data_table_render_bind_list = TF_gpu_data_table_render_bind_list<
        TF_uncacheable_pool_gpu_data_table,
        Fs_element__...
    >;
}