#include <nre/io/file_system.hpp>
#include <nre/io/windows_file_system.hpp>
#include <nre/io/file_saver_system.hpp>
#include <nre/io/file_loader_system.hpp>



namespace nre
{
    TK<A_file_system> A_file_system::instance_p_;



    A_file_system::A_file_system()
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();

        saver_system_p_ = TU<F_file_saver_system>()();
        loader_system_p_ = TU<F_file_loader_system>()();
    }
    A_file_system::~A_file_system()
    {
    }



    bool A_file_system::is_exists(const G_string& path) const
    {
        return (
            is_file_exists(path)
            || is_directory_exists(path)
        );
    }

    bool A_file_system::is_file_exists(const G_string& path) const
    {
        std::ifstream file_stream(path.c_str());
        return file_stream.good();
    }
    eastl::optional<TG_vector<u8>> A_file_system::read_file(const G_string& path) const
    {
        TG_vector<u8> result;

        std::ifstream file_stream(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        // Compute the file size
        std::ifstream::pos_type file_size = file_stream.tellg();
        file_stream.seekg(0, std::ios::beg);

        if (file_size == -1)
            return eastl::nullopt;

        // Read asset buffer
        if (file_size)
        {
            result.resize(file_size * sizeof(char));
            file_stream.read((char*)result.data(), file_size);
        }

        return std::move(result);
    }
    void A_file_system::write_file(const G_string& path, const TG_span<u8>& data) const
    {
        std::ofstream file_stream(path.c_str(), std::ios::binary);

        file_stream.write(
            (const char*)(data.data()),
            data.size()
        );
    }

    void A_file_system::ensure_directory(const G_string& path) const
    {
        if(is_directory_exists(path))
            return;

        make_directory(path);
    }



    TU<A_file_system> H_file_system::create_internal()
    {
#ifdef EA_PLATFORM_WINDOWS
        return TU<F_windows_file_system>()();
#endif
    }
}