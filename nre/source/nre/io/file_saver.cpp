#include <nre/io/file_saver.hpp>
#include <nre/io/file_system.hpp>



namespace nre
{
    A_file_saver::A_file_saver()
    {
    }
    A_file_saver::~A_file_saver()
    {
    }



    F_string_file_saver::F_string_file_saver()
    {
    }
    F_string_file_saver::~F_string_file_saver()
    {
    }

    b8 F_string_file_saver::save(const G_string& abs_path, const G_string& data, u32 flags)
    {
        NRE_FILE_SYSTEM()->write_file(
            abs_path,
            { (u8*)data.data(), data.size() }
        );
        return true;
    }
}