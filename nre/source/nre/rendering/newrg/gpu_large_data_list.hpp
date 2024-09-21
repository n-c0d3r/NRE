#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/render_system.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    class F_render_resource;



    template<typename F_element__>
    class NRE_API TF_gpu_large_data_list final
    {
    public:
        using F_element = F_element__;



    private:
        ED_resource_flag flags_ = ED_resource_flag::NONE;
        ED_resource_heap_type heap_type_ = ED_resource_heap_type::GREAD_GWRITE;
        ED_resource_state initial_state_ = ED_resource_state::COMMON;
        sz element_count_ = 0;
        sz page_capacity_in_elements_ = 0;
        TG_vector<U_buffer_handle> page_p_vector_;
        TG_vector<F_render_resource*> rg_page_p_vector_;
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(F_debug_name name_);

    public:
        NCPP_FORCE_INLINE ED_resource_flag flags() const noexcept { return flags_; }
        NCPP_FORCE_INLINE ED_resource_heap_type heap_type() const noexcept { return heap_type_; }
        NCPP_FORCE_INLINE sz element_count() const noexcept { return element_count_; }
        NCPP_FORCE_INLINE sz page_capacity_in_elements() const noexcept { return page_capacity_in_elements_; }
        NCPP_FORCE_INLINE const auto& page_p_vector() const noexcept { return page_p_vector_; }
        NCPP_FORCE_INLINE const auto& rg_page_p_vector() const noexcept { return rg_page_p_vector_; }
        NCPP_FORCE_INLINE sz page_count() const noexcept { return page_p_vector_.size(); }
        NCPP_FORCE_INLINE sz usuable_page_count() const noexcept
        {
            return (
                (element_count_ / page_capacity_in_elements_)
                + (element_count_ % page_capacity_in_elements_) ? 1 : 0
            );
        }
        NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(NCPP_FORCE_INLINE const F_debug_name& name() const noexcept { return name_; });



    public:
        TF_gpu_large_data_list() = default;
        TF_gpu_large_data_list(
            ED_resource_flag flags,
            ED_resource_heap_type heap_type,
            ED_resource_state initial_state,
            sz element_count,
            sz page_capacity_in_elements = 1024,
            sz page_count = 0
            NRE_OPTIONAL_DEBUG_PARAM(const F_debug_name& name = "")
        ) :
            flags_(flags),
            heap_type_(heap_type),
            initial_state_(initial_state),
            element_count_(element_count),
            page_capacity_in_elements_(page_capacity_in_elements)
            NRE_OPTIONAL_DEBUG_PARAM(name_(name))
        {
            sz new_page_count = eastl::max<sz>(
                (element_count_ / page_capacity_in_elements_) + ((element_count_ % page_capacity_in_elements_) ? 1 : 0),
                page_count
            );
            page_p_vector_.reserve(new_page_count);
            for(u32 i = 0; i < new_page_count; ++i)
            {
                page_p_vector_.push_back(
                    create_page_internal()
                );
                NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                    page_p_vector_[i]->set_debug_name(
                        name_
                        + ".pages["
                        + G_to_string(i).c_str()
                        + "]"
                    )
                );
            }
        }
        ~TF_gpu_large_data_list()
        {
            reset();
        }

        TF_gpu_large_data_list(const TF_gpu_large_data_list& x) :
            flags_(x.flags_),
            heap_type_(x.heap_type_),
            initial_state_(x.initial_state_),
            element_count_(x.element_count_),
            page_capacity_in_elements_(x.page_capacity_in_elements_),
            page_p_vector_(x.page_p_vector_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);
        }
        TF_gpu_large_data_list& operator = (const TF_gpu_large_data_list& x)
        {
            flags_ = x.flags_;
            heap_type_ = x.heap_type_;
            initial_state_ = x.initial_state_;
            element_count_ = x.element_count_;
            page_capacity_in_elements_ = x.page_capacity_in_elements_;
            page_p_vector_ = x.page_p_vector_;

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            return *this;
        }

        TF_gpu_large_data_list(TF_gpu_large_data_list&& x) :
            flags_(x.flags_),
            heap_type_(x.heap_type_),
            initial_state_(x.initial_state_),
            element_count_(x.element_count_),
            page_capacity_in_elements_(x.page_capacity_in_elements_),
            page_p_vector_(x.page_p_vector_)
        {
            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();
        }
        TF_gpu_large_data_list& operator = (TF_gpu_large_data_list&& x)
        {
            flags_ = x.flags_;
            heap_type_ = x.heap_type_;
            initial_state_ = x.initial_state_;
            element_count_ = x.element_count_;
            page_capacity_in_elements_ = x.page_capacity_in_elements_;
            page_p_vector_ = eastl::move(x.page_p_vector_);

            NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(name_ = x.name_);

            x.reset();

            return *this;
        }



    private:
        U_buffer_handle create_page_internal()
        {
            return H_buffer::create_committed(
                NRE_MAIN_DEVICE(),
                page_capacity_in_elements_,
                sizeof(F_element__),
                flags_,
                heap_type_,
                initial_state_
            );
        }



    public:
        NCPP_FORCE_INLINE sz page_index(sz element_index) const noexcept
        {
            return (element_index / page_capacity_in_elements_);
        }
        NCPP_FORCE_INLINE sz local_element_index(sz element_index) const noexcept
        {
            return (element_index % page_capacity_in_elements_);
        }
        NCPP_FORCE_INLINE K_valid_buffer_handle page_p(sz page_index) const noexcept
        {
            return NCPP_FOH_VALID(page_p_vector_[page_index]);
        }
        NCPP_FORCE_INLINE F_render_resource* rg_page_p(sz page_index) const noexcept
        {
            return rg_page_p_vector_[page_index];
        }

    public:
        void reset()
        {
            set_element_count(0);
            set_page_count(0);

            flags_ = ED_resource_flag::NONE;
            heap_type_ = ED_resource_heap_type::DEFAULT;
            initial_state_ = ED_resource_state::COMMON;
        }

    public:
        void set_element_count(sz new_element_count)
        {
            element_count_ = new_element_count;

            sz new_page_count = eastl::max<sz>(
                usuable_page_count(),
                page_count()
            );

            {
                u32 i = page_count();
                page_p_vector_.reserve(new_page_count);
                for(; i < new_page_count; ++i)
                {
                    page_p_vector_.push_back(
                        create_page_internal()
                    );
                    NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                        page_p_vector_[i]->set_debug_name(
                            name_
                            + ".pages["
                            + G_to_string(i).c_str()
                            + "]"
                        )
                    );
                }
            }
        }
        void set_page_count(sz new_page_count)
        {
            new_page_count = eastl::max<sz>(
                usuable_page_count(),
                new_page_count
            );

            {
                u32 i = page_count();
                page_p_vector_.resize(new_page_count);
                for(; i < new_page_count; ++i)
                {
                    page_p_vector_.push_back(
                        create_page_internal()
                    );
                    NRHI_ENABLE_IF_DRIVER_DEBUGGER_ENABLED(
                        page_p_vector_[i]->set_debug_name(
                            name_
                            + ".pages["
                            + G_to_string(i).c_str()
                            + "]"
                        )
                    );
                }
            }
        }

    public:
        void RG_begin_register()
        {
            rg_page_p_vector_.resize(0);
        }
        void RG_end_register()
        {
            auto render_graph_p = F_render_graph::instance_p();

            rg_page_p_vector_.resize(usuable_page_count());
            for(u32 i = 0; i < usuable_page_count(); ++i)
            {
                auto& rg_page_p = rg_page_p_vector_[i];

                auto rhi_p = page_p(i);

                rg_page_p = render_graph_p->create_permanent_resource(
                    rhi_p,
                    initial_state_
                    NRE_OPTIONAL_DEBUG_PARAM(rhi_p->debug_name().c_str())
                );
            }
        }
    };
}