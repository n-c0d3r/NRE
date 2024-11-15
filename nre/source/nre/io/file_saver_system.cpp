#include <nre/io/file_saver_system.hpp>
#include <nre/io/file_saver.hpp>


namespace nre
{
    TK<F_file_saver_system> F_file_saver_system::instance_p_;



    F_file_saver_system::F_file_saver_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        T_register_saver<F_string_file_saver>();
    }
    F_file_saver_system::~F_file_saver_system()
    {
    }



    b8 F_file_saver_system::has_saver(u64 data_type_hash_code)
    {
        return saver_map_.find(data_type_hash_code) != saver_map_.end();
    }

    TK_valid<A_file_saver> F_file_saver_system::find_saver(u64 data_type_hash_code)
    {
        auto it = saver_map_.find(data_type_hash_code);

        NCPP_ASSERT(it != saver_map_.end());

        return NCPP_FOH_VALID(it->second);
    }



    b8 F_file_saver_system::save(const G_string& abs_path, const TG_span<u8>& data, TKPA_valid<A_file_saver> saver_p, u32 flags)
    {
        return saver_p->save_internal(
            abs_path,
            data,
            flags
        );
    }
}
