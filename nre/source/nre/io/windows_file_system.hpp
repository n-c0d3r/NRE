#pragma once

#include <nre/prerequisites.hpp>

#include <nre/io/file_system.hpp>

#ifdef EA_PLATFORM_WINDOWS
#include <shlwapi.h>
#endif



#ifdef EA_PLATFORM_WINDOWS
namespace nre
{
    class NRE_API F_windows_file_system : public A_file_system
    {
    public:
        friend class H_file_system;

    protected:
        F_windows_file_system();

    public:
        virtual ~F_windows_file_system() override;

    public:
        NCPP_OBJECT(F_windows_file_system);



    public:
        virtual bool is_directory_exists(const G_string& path) const override;
        virtual void make_directory(const G_string& path) const override;
    };
}
#endif