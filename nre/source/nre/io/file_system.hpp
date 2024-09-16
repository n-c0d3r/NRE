#pragma once

#include <nre/prerequisites.hpp>



namespace nre
{
    class F_file_saver_system;
    class F_file_loader_system;



    class NRE_API A_file_system
    {
    private:
        static TK<A_file_system> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<A_file_system> instance_p() { return (TKPA_valid<A_file_system>)instance_p_; }



    private:
        TU<F_file_saver_system> saver_system_p_;
        TU<F_file_loader_system> loader_system_p_;



    protected:
        A_file_system();

    public:
        virtual ~A_file_system();

    public:
        NCPP_OBJECT(A_file_system);



    public:
        bool is_exists(const G_string& path) const;

    public:
        virtual bool is_file_exists(const G_string& path) const;
        virtual eastl::optional<TG_vector<u8>> read_file(const G_string& path) const;
        virtual void write_file(const G_string& path, const TG_span<u8>& data) const;

    public:
        virtual bool is_directory_exists(const G_string& path) const = 0;
        virtual void make_directory(const G_string& path) const = 0;

    public:
        void ensure_directory(const G_string& path) const;
    };



    class NRE_API H_file_system
    {
    public:
        friend class F_application;



    private:
        static TU<A_file_system> create_internal();
    };
}



#define NRE_FILE_SYSTEM() nre::A_file_system::instance_p()