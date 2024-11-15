#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class NRE_API A_file_loader
    {
    public:
        friend class F_file_loader_system;



    protected:
        A_file_loader();

    public:
        virtual ~A_file_loader();

    public:
        NCPP_OBJECT(A_file_loader);



    public:
        virtual b8 load_internal(const G_string& abs_path, const TG_span<u8>& data, u32 flags = 0) = 0;
    };



    template<typename F_data__>
    class TA_file_loader : public A_file_loader
    {
    public:
        using F_data = F_data__;



    protected:
        TA_file_loader()
        {
        }

    public:
        virtual ~TA_file_loader()
        {
        }

    public:
        NCPP_OBJECT(TA_file_loader);



    protected:
        virtual b8 load(const G_string& abs_path, F_data& data, u32 flags = 0) = 0;

    protected:
        b8 load_internal(const G_string& abs_path, const TG_span<u8>& data, u32 flags = 0) final override
        {
            return load(abs_path, *((F_data*)(data.data())), flags);
        }
    };



    class NRE_API F_string_file_loader final : public TA_file_loader<G_string>
    {
    public:
        F_string_file_loader();
        ~F_string_file_loader() override;

    public:
        NCPP_OBJECT(F_string_file_loader);

    protected:
        b8 load(const G_string& abs_path, G_string& data, u32 flags) override;
    };
}