#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg::abytek_geometry
{
    class NRE_API F_actor_data_stream final
    {
    private:
        u32 stride_ = 0;
        ED_resource_heap_type heap_type_ = ED_resource_heap_type::DEFAULT;

    public:
        NCPP_FORCE_INLINE u32 stride() const noexcept { return stride_; }
        NCPP_FORCE_INLINE ED_resource_heap_type heap_type() const noexcept { return heap_type_; }



    public:
        F_actor_data_stream(
            u32 stride,
            ED_resource_heap_type heap_type = ED_resource_heap_type::DEFAULT
        );
        ~F_actor_data_stream();
    };
}