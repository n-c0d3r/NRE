#include <nre/prerequisites.hpp>
#include <nre/rendering/newrg/intermediate_descriptor_manager.hpp>
#include <nre/rendering/render_system.hpp>

using namespace nre;



namespace
{
    TU<A_sampler_state> create_intermediate(
        TKPA_valid<A_device> device_p,
        const F_sampler_state_desc& desc
    )
    {
        ED_descriptor_heap_type heap_type = ED_descriptor_heap_type::SAMPLER;

        u32 descriptor_increment_size = NRE_MAIN_DEVICE()->descriptor_increment_size(heap_type);

        newrg::F_descriptor_allocation descriptor_allocation = newrg::F_intermediate_descriptor_manager::instance_p()->allocate(
            1,
            heap_type
        );

        auto& page = descriptor_allocation.allocator_p->pages()[descriptor_allocation.page_index];

        F_descriptor_cpu_address cpu_address = page.base_cpu_address() + descriptor_allocation.placed_range.begin * descriptor_increment_size;

        H_descriptor::initialize_sampler_state(
            NCPP_FOH_VALID(page.heap_p),
            cpu_address,
            desc
        );

        return H_sampler_state::create_with_managed_descriptor(
            device_p,
            desc,
            {
                .handle = {
                    .cpu_address = cpu_address
                },
                .heap_p = page.heap_p
            }
        );
    }
}



TU<A_sampler_state> NRHI_DRIVER_ALTERNATIVE(nrhi, H_sampler_state)::create(
    TKPA_valid<A_device> device_p,
    const F_sampler_state_desc& desc
)
{
    return create_intermediate(device_p, desc);
}

void NRHI_DRIVER_ALTERNATIVE(nrhi, H_sampler_state)::release_driver_specific_implementation(
    TKPA_valid<A_sampler_state> sampler_state_p
)
{
    const F_descriptor& descriptor = sampler_state_p->managed_descriptor();
    NCPP_ASSERT(descriptor);

    const F_sampler_state_desc& desc = sampler_state_p->desc();

    newrg::F_intermediate_descriptor_manager::instance_p()->deallocate_with_cpu_address(
        descriptor.handle.cpu_address,
        ED_descriptor_heap_type::SAMPLER
    );
}