#pragma once

#include <nre/prerequisites.hpp>

#include <nre/utilities/pool_allocator_estimator.hpp>



namespace nre
{
    class NRE_API F_pool_data_distributor final
    {
    private:
        sz page_capacity_ = 0;
        TG_vector<F_pool_allocator_estimator> pages_;
        asz next_id_ = 0;
        asz id_count_ = 0;

        NCPP_ENABLE_IF_ASSERTION_ENABLED(b8 is_in_register_phase_ = false);

    public:
        NCPP_FORCE_INLINE sz page_capacity() const noexcept { return page_capacity_; }
        NCPP_FORCE_INLINE const auto& pages() const noexcept { return pages_; }
        NCPP_FORCE_INLINE sz page_count() const noexcept { return pages_.size(); }
        NCPP_FORCE_INLINE sz capacity() const noexcept { return page_count() * page_capacity_; }
        NCPP_FORCE_INLINE sz id_count() const noexcept { return id_count_; }



    public:
        F_pool_data_distributor() = default;
        F_pool_data_distributor(sz page_capacity) :
            page_capacity_(page_capacity)
        {
        }
        ~F_pool_data_distributor()
        {
            reset();
        }

        F_pool_data_distributor(const F_pool_data_distributor& x) :
            page_capacity_(x.page_capacity_),
            pages_(x.pages_),
            next_id_(x.next_id_.load())
        {
        }
        F_pool_data_distributor& operator = (const F_pool_data_distributor& x)
        {
            page_capacity_ = x.page_capacity_;
            pages_ = x.pages_;
            next_id_ = x.next_id_.load();

            return *this;
        }

        F_pool_data_distributor(F_pool_data_distributor&& x) :
            page_capacity_(x.page_capacity_),
            pages_(x.pages_),
            next_id_(x.next_id_.load())
        {
            x.reset();
        }
        F_pool_data_distributor& operator = (F_pool_data_distributor&& x)
        {
            page_capacity_ = x.page_capacity_;
            pages_ = eastl::move(x.pages_);
            next_id_ = x.next_id_.load();

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
            next_id_ = 0;
        }

    public:
        void begin_register()
        {
            NCPP_ASSERT(!is_in_register_phase_);
            NCPP_ENABLE_IF_ASSERTION_ENABLED(is_in_register_phase_ = true);
        }
        void end_register()
        {
            NCPP_ASSERT(is_in_register_phase_);
            NCPP_ENABLE_IF_ASSERTION_ENABLED(is_in_register_phase_ = false);

            auto next_id = next_id_.load();

            sz last_page_count = pages_.size();
            sz new_page_count = (
                (next_id / page_capacity_)
                + ((next_id % page_capacity_) ? 1 : 0)
            );

            for(sz i = last_page_count; i < new_page_count; ++i)
            {
                create_page_internal();
                for(sz j = i * page_capacity_, end_j = next_id_.load(); j < end_j; ++j)
                {
                    pages_[i].try_allocate();
                }
            }

            next_id_ = new_page_count * page_capacity_;
        }

    public:
        sz register_id()
        {
            NCPP_ASSERT(is_in_register_phase_);

            ++id_count_;

            sz page_count = pages_.size();
            for(sz i = page_count - 1; i != sz(-1); --i)
            {
                auto& page = pages_[i];

                if(auto local_id_opt = page.try_allocate())
                    return local_id_opt.value() + i * page_capacity_;
            }

            return next_id_.fetch_add(1);
        }
        void deregister_id(sz id)
        {
            NCPP_ASSERT(is_in_register_phase_);

            --id_count_;

            sz page_index = id / page_capacity_;

            NCPP_ASSERT(page_index < pages_.size()) << "can't register and deregister id in the same begin/end register phase";

            pages_[page_index].deallocate(id - page_index * page_capacity_);
        }
    };
}