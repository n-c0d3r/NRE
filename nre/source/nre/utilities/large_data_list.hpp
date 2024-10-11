#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    template<typename F_element__>
    class NRE_API TF_large_data_list final
    {
    public:
        using F_element = F_element__;
        using F_page = TG_vector<F_element>;



    private:
        sz element_count_ = 0;
        sz page_capacity_in_elements_ = 0;
        TG_vector<F_page> pages_;

    public:
        NCPP_FORCE_INLINE sz element_count() const noexcept { return element_count_; }
        NCPP_FORCE_INLINE sz page_capacity_in_elements() const noexcept { return page_capacity_in_elements_; }
        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }
        NCPP_FORCE_INLINE sz page_count() const noexcept { return pages_.size(); }
        NCPP_FORCE_INLINE sz usuable_page_count() const noexcept
        {
            return (
                (element_count_ / page_capacity_in_elements_)
                + ((element_count_ % page_capacity_in_elements_) ? 1 : 0)
            );
        }



    public:
        TF_large_data_list() = default;
        TF_large_data_list(sz element_count, sz page_capacity_in_elements = 1024, sz page_count = 0) :
            element_count_(element_count),
            page_capacity_in_elements_(page_capacity_in_elements)
        {
            sz new_page_count = eastl::max<sz>(
                (element_count_ / page_capacity_in_elements_) + ((element_count_ % page_capacity_in_elements_) ? 1 : 0),
                page_count
            );
            pages_.reserve(new_page_count);
            for(u32 i = 0; i < new_page_count; ++i)
            {
                pages_.emplace_back(page_capacity_in_elements_);
            }
        }
        ~TF_large_data_list()
        {
            reset();
        }

        TF_large_data_list(const TF_large_data_list& x) :
            element_count_(x.element_count_),
            page_capacity_in_elements_(x.page_capacity_in_elements_),
            pages_(x.pages_)
        {
        }
        TF_large_data_list& operator = (const TF_large_data_list& x)
        {
            element_count_ = x.element_count_;
            page_capacity_in_elements_ = x.page_capacity_in_elements_;
            pages_ = x.pages_;

            return *this;
        }

        TF_large_data_list(TF_large_data_list&& x) :
            element_count_(x.element_count_),
            page_capacity_in_elements_(x.page_capacity_in_elements_),
            pages_(x.pages_)
        {
            x.reset();
        }
        TF_large_data_list& operator = (TF_large_data_list&& x)
        {
            element_count_ = x.element_count_;
            page_capacity_in_elements_ = x.page_capacity_in_elements_;
            pages_ = eastl::move(x.pages_);

            x.reset();

            return *this;
        }



    public:
        NCPP_FORCE_INLINE F_element& element(sz page_index, sz local_element_index) noexcept
        {
            return pages_[page_index][local_element_index];
        }
        NCPP_FORCE_INLINE const F_element& element(sz page_index, sz local_element_index) const noexcept
        {
            return pages_[page_index][local_element_index];
        }
        NCPP_FORCE_INLINE sz page_index(sz element_index) const noexcept
        {
            return (element_index / page_capacity_in_elements_);
        }
        NCPP_FORCE_INLINE sz local_element_index(sz element_index) const noexcept
        {
            return (element_index % page_capacity_in_elements_);
        }
        NCPP_FORCE_INLINE F_element& element(sz element_index) noexcept
        {
            return element(
                page_index(element_index),
                local_element_index(element_index)
            );
        }
        NCPP_FORCE_INLINE const F_element& element(sz element_index) const noexcept
        {
            return element(
                page_index(element_index),
                local_element_index(element_index)
            );
        }
        NCPP_FORCE_INLINE auto& page(sz page_index) noexcept
        {
            return pages_[page_index];
        }
        NCPP_FORCE_INLINE const auto& page(sz page_index) const noexcept
        {
            return pages_[page_index];
        }
        NCPP_FORCE_INLINE F_element& operator [] (sz element_index) noexcept
        {
            return element(element_index);
        }
        NCPP_FORCE_INLINE const F_element& operator [] (sz element_index) const noexcept
        {
            return element(element_index);
        }

    public:
        void reset()
        {
            set_element_count(0);
            set_page_count(0);
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
                pages_.resize(new_page_count);
                for(; i < new_page_count; ++i)
                {
                    pages_[i] = F_page(page_capacity_in_elements_);
                }
            }
        }
        void set_page_count(sz new_page_count)
        {
            new_page_count = eastl::max<sz>(
                (element_count_ / page_capacity_in_elements_) + ((element_count_ % page_capacity_in_elements_) ? 1 : 0),
                new_page_count
            );

            {
                u32 i = page_count();
                pages_.resize(new_page_count);
                for(; i < new_page_count; ++i)
                {
                    pages_[i] = F_page(page_capacity_in_elements_);
                }
            }
        }
    };
}