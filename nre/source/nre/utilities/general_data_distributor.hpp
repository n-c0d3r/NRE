#pragma once

#include <nre/prerequisites.hpp>

#include <nre/utilities/general_allocator_estimator.hpp>



namespace nre
{
    class NRE_API F_general_data_distributor final
    {
    private:
        sz page_capacity_ = 0;
        TG_vector<F_general_allocator_estimator> pages_;
        sz id_count_;

    public:
        NCPP_FORCE_INLINE sz page_capacity() const noexcept { return page_capacity_; }
        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }
        NCPP_FORCE_INLINE sz page_count() const noexcept { return pages_.size(); }
        NCPP_FORCE_INLINE sz capacity() const noexcept { return page_count() * page_capacity_; }
        NCPP_FORCE_INLINE sz id_count() const noexcept { return id_count_; }



    public:
        F_general_data_distributor() = default;
        F_general_data_distributor(sz page_capacity) :
            page_capacity_(page_capacity)
        {
        }
        ~F_general_data_distributor()
        {
            reset();
        }

        F_general_data_distributor(const F_general_data_distributor& x) :
            page_capacity_(x.page_capacity_),
            pages_(x.pages_)
        {
        }
        F_general_data_distributor& operator = (const F_general_data_distributor& x)
        {
            page_capacity_ = x.page_capacity_;
            pages_ = x.pages_;

            return *this;
        }

        F_general_data_distributor(F_general_data_distributor&& x) :
            page_capacity_(x.page_capacity_),
            pages_(x.pages_)
        {
            x.reset();
        }
        F_general_data_distributor& operator = (F_general_data_distributor&& x)
        {
            page_capacity_ = x.page_capacity_;
            pages_ = eastl::move(x.pages_);

            x.reset();

            return *this;
        }



    private:
        void create_page_internal()
        {
            pages_.emplace_back(page_capacity_);
        }



    public:
        NCPP_FORCE_INLINE auto& page(sz page_index) noexcept
        {
            return pages_[page_index];
        }
        NCPP_FORCE_INLINE const auto& page(sz page_index) const noexcept
        {
            return pages_[page_index];
        }

    public:
        void reset()
        {
            page_capacity_ = 0;
            pages_.clear();
        }

    public:
        sz register_id(sz count = 1, sz alignment = 1)
        {
            NCPP_ASSERT((count + alignment - 1) <= page_capacity_) << "count too big";

            sz page_count = pages_.size();
            for(sz i = 0; i < page_count; ++i)
            {
                auto& page = pages_[i];

                if(auto local_id_opt = page.try_allocate(count, alignment))
                {
                    return local_id_opt.value() + i * page_capacity_;
                }
            }

            create_page_internal();

            id_count_ += count;

            return page_count * page_capacity_ + pages_.back().try_allocate(count, alignment).value();
        }
        void deregister_id(sz id)
        {
            sz page_index = id / page_capacity_;

            NCPP_ASSERT(page_index < pages_.size());

            id_count_ -= pages_[page_index].deallocate(id - page_index * page_capacity_);
        }
    };
}