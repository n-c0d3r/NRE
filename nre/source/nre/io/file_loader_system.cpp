#include <nre/io/file_loader_system.hpp>
#include <nre/io/file_loader.hpp>


namespace nre
{
    TK<F_file_loader_system> F_file_loader_system::instance_p_;



    F_file_loader_system::F_file_loader_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
    F_file_loader_system::~F_file_loader_system()
    {
    }



    b8 F_file_loader_system::has_loader(u64 data_type_hash_code)
    {
        return loader_map_.find(data_type_hash_code) != loader_map_.end();
    }

    TK_valid<A_file_loader> F_file_loader_system::find_loader(u64 data_type_hash_code)
    {
        auto it = loader_map_.find(data_type_hash_code);

        NCPP_ASSERT(it != loader_map_.end());

        return NCPP_FOH_VALID(it->second);
    }



    b8 F_file_loader_system::load(const G_string& abs_path, const TG_span<u8>& data, TKPA_valid<A_file_loader> loader_p, u32 flags)
    {
        return loader_p->load_internal(
            abs_path,
            data,
            flags
        );
    }
}
