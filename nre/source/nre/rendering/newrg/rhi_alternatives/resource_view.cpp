#include <nre/prerequisites.hpp>
#include <nre/rendering/newrg/intermediate_descriptor_manager.hpp>

#include "nre/rendering/render_system.hpp"

using namespace nre;



namespace
{
    ED_descriptor_heap_type resource_view_type_to_descriptor_heap_type(ED_resource_view_type resource_view_type)
    {
        NRHI_ENUM_SWITCH(
            resource_view_type,
            NRHI_ENUM_CASE(
                ED_resource_view_type::SHADER_RESOURCE,
                return ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::UNORDERED_ACCESS,
                return ED_descriptor_heap_type::CONSTANT_BUFFER_SHADER_RESOURCE_UNORDERED_ACCESS
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::RENDER_TARGET,
                return ED_descriptor_heap_type::RENDER_TARGET
            )
            NRHI_ENUM_CASE(
                ED_resource_view_type::DEPTH_STENCIL,
                return ED_descriptor_heap_type::DEPTH_STENCIL
            )
        );
    }

    TU<A_resource_view> create_intermediate(
        TKPA_valid<A_device> device_p,
        const F_resource_view_desc& desc
    )
    {
        ED_descriptor_heap_type heap_type = resource_view_type_to_descriptor_heap_type(desc.type);

        u32 descriptor_increment_size = NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type);

        newrg::F_descriptor_allocation descriptor_allocation = newrg::F_intermediate_descriptor_manager::instance_p()->allocate(
            1,
            heap_type
        );

        auto& page = descriptor_allocation.allocator_p->pages()[descriptor_allocation.page_index];

        F_descriptor_cpu_address cpu_address = page.base_cpu_address() + descriptor_allocation.placed_range.begin * descriptor_increment_size;
        F_descriptor_gpu_address gpu_address = page.base_gpu_address() + descriptor_allocation.placed_range.begin * descriptor_increment_size;

        H_descriptor::initialize_resource_view(
            NCPP_FOH_VALID(page.heap_p),
            cpu_address,
            desc
        );

        return H_resource_view::create_with_managed_descriptor(
            device_p,
            desc,
            {
                .handle = {
                    .cpu_address = cpu_address,
                    .gpu_address = gpu_address
                },
                .heap_p = page.heap_p
            }
        );
    }
}



TU<A_resource_view> NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return create_intermediate(device_p, desc);
}

U_srv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_srv(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return { create_intermediate(device_p, desc) };
}
U_uav_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_uav(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return { create_intermediate(device_p, desc) };
}
U_rtv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_rtv(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return { create_intermediate(device_p, desc) };
}
U_dsv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_dsv(
    TKPA_valid<A_device> device_p,
    const F_resource_view_desc& desc
)
{
    return { create_intermediate(device_p, desc) };
}

U_srv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_srv(
    TKPA_valid<A_resource> resource_p
)
{
    return { create_intermediate(resource_p->device_p(), { .resource_p = resource_p.no_requirements() }) };
}
U_uav_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_uav(
    TKPA_valid<A_resource> resource_p
)
{
    return { create_intermediate(resource_p->device_p(), { .resource_p = resource_p.no_requirements() }) };
}
U_rtv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_rtv(
    TKPA_valid<A_resource> resource_p
)
{
    return { create_intermediate(resource_p->device_p(), { .resource_p = resource_p.no_requirements() }) };
}
U_dsv_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::create_default_dsv(
    TKPA_valid<A_resource> resource_p
)
{
    return { create_intermediate(resource_p->device_p(), { .resource_p = resource_p.no_requirements() }) };
}
void NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource_view)::release_driver_specific_implementation(
    TKPA_valid<A_resource_view> resource_view_p
)
{
    const F_descriptor& descriptor = resource_view_p->managed_descriptor();
    NCPP_ASSERT(descriptor);

    const F_resource_view_desc& desc = resource_view_p->desc();

    newrg::F_intermediate_descriptor_manager::instance_p()->deallocate_with_cpu_address(
        descriptor.handle.cpu_address,
        resource_view_type_to_descriptor_heap_type(desc.type)
    );
}