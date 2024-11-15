#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class A_file_saver;



    class NRE_API F_file_saver_system
    {
    private:
        static TK<F_file_saver_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_file_saver_system> instance_p() { return (TKPA_valid<F_file_saver_system>)instance_p_; }



    private:
        TG_vector<TU<A_file_saver>> saver_p_vector_;
        TG_unordered_map<u64, TK<A_file_saver>> saver_map_;



    public:
        F_file_saver_system();
        virtual ~F_file_saver_system();

    public:
        NCPP_OBJECT(F_file_saver_system);
        template<typename F_file_saver__>
        requires T_is_object_down_castable<F_file_saver__, A_file_saver>
        TK_valid<F_file_saver__> T_register_saver(auto&&... args)
        {
            NCPP_ASSERT(!has_saver(T_type_hash_code<typename F_file_saver__::F_data>));

            auto saver_p = TU<F_file_saver__>()(NCPP_FORWARD(args)...);
            auto keyed_saver_p = NCPP_FOH_VALID(saver_p);

            saver_p_vector_.push_back(
                std::move(saver_p)
            );

            saver_map_[T_type_hash_code<typename F_file_saver__::F_data>] = keyed_saver_p.no_requirements();

            return keyed_saver_p;
        }

        b8 has_saver(u64 data_type_hash_code);
        template<typename F_data__>
        TK_valid<A_file_saver> T_has_saver()
        {
            return has_saver(
                T_type_hash_code<F_data__>
            );
        }

        TK_valid<A_file_saver> find_saver(u64 data_type_hash_code);
        template<typename F_data__>
        TK_valid<A_file_saver> T_find_saver()
        {
            return find_saver(
                T_type_hash_code<F_data__>
            );
        }

    public:
        b8 save(const G_string& abs_path, const TG_span<u8>& data, TKPA_valid<A_file_saver> saver_p, u32 flags = 0);
        template<typename F_data__>
        b8 T_save(const G_string& abs_path, const F_data__& data, u32 flags = 0)
        {
            return save(
                abs_path,
                TG_span<u8> { (u8*)&data, sizeof(F_data__) },
                T_find_saver<F_data__>(),
                flags
            );
        }
    };
}



#define NRE_FILE_SAVER_SYSTEM() nre::F_file_saver_system::instance_p()