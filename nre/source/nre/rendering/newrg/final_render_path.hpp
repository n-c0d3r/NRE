#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_path.hpp>



namespace nre::newrg
{
    class NRE_API F_final_render_path : public F_render_path
    {
    public:
        F_final_render_path();
        virtual ~F_final_render_path();

    public:
        NCPP_OBJECT(F_final_render_path);



    public:
        virtual void RG_begin_register() override;
        virtual void RG_end_register() override;
    };
}