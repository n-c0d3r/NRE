#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class NRE_API A_file_saver
    {
    public:
        friend class F_file_saver_system;



    protected:
        A_file_saver();

    public:
        virtual ~A_file_saver();

    public:
        NCPP_OBJECT(A_file_saver);



    public:
        virtual b8 save_internal(const G_string& abs_path, const TG_span<u8>& data, u32 flags = 0) = 0;
    };



    template<typename F_data__>
    class TA_file_saver : public A_file_saver
    {
    public:
        using F_data = F_data__;



    protected:
        TA_file_saver()
        {
        }

    public:
        virtual ~TA_file_saver()
        {
        }

    public:
        NCPP_OBJECT(TA_file_saver);



    protected:
        virtual b8 save(const G_string& abs_path, const F_data& data, u32 flags = 0) = 0;

    protected:
        b8 save_internal(const G_string& abs_path, const TG_span<u8>& data, u32 flags = 0) final override
        {
            return save(abs_path, *((const F_data*)(data.data())), flags);
        }
    };
}