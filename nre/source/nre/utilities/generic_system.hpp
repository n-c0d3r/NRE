#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    template<typename F_value__>
    struct TF_generic_handle
    {
    public:
        using F_value = F_value__;
        using F_iterator = typename TG_list<F_value>::iterator;

    public:
        F_iterator iterator;
    };



    template<typename F_value__>
    class TF_generic_system
    {
    public:
        using F_value = F_value__;
        using F_list = TG_list<F_value__>;
        using F_handle = TF_generic_handle<F_value__>;



    private:
        F_list list_;

    public:
        NCPP_FORCE_INLINE const auto& list() const noexcept { return list_; }



    public:
        TF_generic_system()
        {
        }
        virtual ~TF_generic_system()
        {
        }

    public:
        NCPP_OBJECT(TF_generic_system);

    public:
        void _register(F_handle& handle, auto&& value)
        {
            auto& it = handle.iterator;

            list_.push_back(NCPP_FORWARD(value));
            it = list_.end();
            --it;
        }
        void deregister(F_handle& handle)
        {
            auto& it = handle.iterator;

            list_.erase(it);
            it = list_.end();
        }

    public:
        void for_each(auto&& callback) const
        {
            for(auto& value : list_)
            {
                callback(value);
            }
        }
    };
}