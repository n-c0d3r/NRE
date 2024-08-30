#pragma once

#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class A_binder_signature;



    class F_binder_signature_manager
    {
    private:
        static TK<F_binder_signature_manager> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_binder_signature_manager> instance_p() { return (TKPA_valid<F_binder_signature_manager>)instance_p_; }



    private:
        TG_vector<TU<A_binder_signature>> owned_signature_p_vector_;
        TG_unordered_map<G_string, TK<A_binder_signature>> name_to_signature_p_;

    public:
        NCPP_FORCE_INLINE const auto& owned_signature_p_vector() const noexcept { return owned_signature_p_vector_; }
        NCPP_FORCE_INLINE const auto& name_to_signature_p() const noexcept { return name_to_signature_p_; }



    public:
        F_binder_signature_manager();
        ~F_binder_signature_manager();

    public:
        NCPP_OBJECT(F_binder_signature_manager);



    public:
        template<class F_signature__>
        TK_valid<F_signature__> T_add(auto&&... args)
        {
            TU<F_signature__> signature_p = TU<F_signature__>()(NCPP_FORWARD(args)...);
            auto keyed_signature_p = signature_p.keyed();

            owned_signature_p_vector_.push_back(std::move(signature_p));
            name_to_signature_p_[keyed_signature_p->nsl_macro_name()] = keyed_signature_p;

            return keyed_signature_p;
        }
    };
}
