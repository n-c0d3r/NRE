#include <nre/rendering/newrg/descriptor_allocation.hpp>
#include <nre/rendering/newrg/descriptor_allocator.hpp>



namespace nre::newrg
{
    F_descriptor_cpu_address F_descriptor_allocation::base_cpu_address() const noexcept
    {
        auto& heap_p = allocator_p->pages()[page_index].heap_p;
        auto device_p = heap_p->device_p();

        return (
            heap_p->base_cpu_address()
            + placed_range.begin * device_p->descriptor_increment_size(allocator_p->heap_type())
        );
    }
    F_descriptor_cpu_address F_descriptor_allocation::base_gpu_address() const noexcept
    {
        auto& heap_p = allocator_p->pages()[page_index].heap_p;
        auto device_p = heap_p->device_p();

        return (
            heap_p->base_gpu_address()
            + placed_range.begin * device_p->descriptor_increment_size(allocator_p->heap_type())
        );
    }
}
