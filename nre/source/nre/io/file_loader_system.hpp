#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class A_file_loader;



    class NRE_API F_file_loader_system
    {
    private:
        static TK<F_file_loader_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_file_loader_system> instance_p() { return (TKPA_valid<F_file_loader_system>)instance_p_; }



    private:
        TG_vector<TU<A_file_loader>> loader_p_vector_;
        TG_unordered_map<u64, TK<A_file_loader>> loader_map_;



    public:
        F_file_loader_system();
        virtual ~F_file_loader_system();

    public:
        NCPP_OBJECT(F_file_loader_system);
        template<typename F_file_loader__>
        requires T_is_object_down_castable<F_file_loader__, A_file_loader>
        TK_valid<F_file_loader__> T_register_loader(auto&&... args)
        {
            NCPP_ASSERT(!has_loader(T_type_hash_code<typename F_file_loader__::F_data>));

            auto loader_p = TU<F_file_loader__>()(NCPP_FORWARD(args)...);
            auto keyed_loader_p = NCPP_FOH_VALID(loader_p);

            loader_p_vector_.push_back(
                std::move(loader_p)
            );

            loader_map_[T_type_hash_code<typename F_file_loader__::F_data>] = keyed_loader_p.no_requirements();

            return keyed_loader_p;
        }

        b8 has_loader(u64 data_type_hash_code);
        template<typename F_data__>
        TK_valid<A_file_loader> T_has_loader()
        {
            return has_loader(
                T_type_hash_code<F_data__>
            );
        }

        TK_valid<A_file_loader> find_loader(u64 data_type_hash_code);
        template<typename F_data__>
        TK_valid<A_file_loader> T_find_loader()
        {
            return find_loader(
                T_type_hash_code<F_data__>
            );
        }

    public:
        b8 load(const G_string& abs_path, const TG_span<u8>& data, TKPA_valid<A_file_loader> loader_p, u32 flags = 0);
        template<typename F_data__>
        b8 T_load(const G_string& abs_path, F_data__& data, u32 flags = 0)
        {
            return load(
                abs_path,
                TG_span<u8> { (u8*)&data, sizeof(F_data__) },
                T_find_loader<F_data__>(),
                flags
            );
        }
    };
}



#define NRE_FILE_LOADER_SYSTEM() nre::F_file_loader_system::instance_p()