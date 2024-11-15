#include <nre/io/file_loader.hpp>
#include <nre/io/file_system.hpp>



namespace nre
{
    A_file_loader::A_file_loader()
    {
    }
    A_file_loader::~A_file_loader()
    {
    }



    F_string_file_loader::F_string_file_loader()
    {
    }
    F_string_file_loader::~F_string_file_loader()
    {
    }

    b8 F_string_file_loader::load(const G_string& abs_path, G_string& data, u32 flags)
    {
        auto buffer_opt = NRE_FILE_SYSTEM()->read_file(abs_path);
        if(!buffer_opt)
            return false;

        auto& buffer = buffer_opt.value();

        data.resize(buffer.size());

        memcpy(
            data.data(),
            buffer.data(),
            buffer.size()
        );

        return true;
    }
}