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
        TG_unordered_map<G_string, TK<A_binder_signature>> nsl_macro_name_to_signature_p_;
        TG_unordered_map<u64, TK<A_binder_signature>> type_hash_code_to_signature_p_;

    public:
        NCPP_FORCE_INLINE const auto& owned_signature_p_vector() const noexcept { return owned_signature_p_vector_; }
        NCPP_FORCE_INLINE const auto& nsl_macro_name_to_signature_p() const noexcept { return nsl_macro_name_to_signature_p_; }
        NCPP_FORCE_INLINE const auto& type_hash_code_to_signature_p() const noexcept { return type_hash_code_to_signature_p_; }



    public:
        F_binder_signature_manager();
        ~F_binder_signature_manager();

    public:
        NCPP_OBJECT(F_binder_signature_manager);



    public:
        void install();

    public:
        template<class F_signature__>
        TK_valid<F_signature__> T_register(auto&&... args)
        {
            TU<F_signature__> signature_p = TU<F_signature__>()(NCPP_FORWARD(args)...);
            auto keyed_signature_p = signature_p.keyed();

            owned_signature_p_vector_.push_back(std::move(signature_p));
            nsl_macro_name_to_signature_p_[keyed_signature_p->nsl_macro_name()] = keyed_signature_p;
            type_hash_code_to_signature_p_[T_type_hash_code<F_signature__>] = keyed_signature_p;

            return NCPP_FOH_VALID(keyed_signature_p);
        }
        template<class F_signature__>
        TK_valid<F_signature__> T_find() const
        {
            return NCPP_FOH_VALID(
                type_hash_code_to_signature_p_.find(T_type_hash_code<F_signature__>)->second.T_cast<F_signature__>()
            );
        }
    };
}
