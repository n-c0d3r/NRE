#pragma once

#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/render_frame_allocator.hpp>
#include <nre/rendering/newrg/indirect_command_batch.hpp>



namespace nre::newrg
{
    class F_indirect_argument_list_layout;



    class NRE_API F_indirect_argument_list final
    {
    private:
        TK<F_indirect_argument_list_layout> layout_p_;
        u32 command_count_ = 0;

        sz size_ = 0;
        TG_span<u8> data_;

    public:
        NCPP_FORCE_INLINE const auto& layout_p() const noexcept { return layout_p_; }
        NCPP_FORCE_INLINE u32 command_count() const noexcept { return command_count_; }

        NCPP_FORCE_INLINE sz size() const noexcept { return size_; }
        NCPP_FORCE_INLINE const auto& data() const noexcept { return data_; }



    public:
        F_indirect_argument_list() = default;
        F_indirect_argument_list(TKPA_valid<F_indirect_argument_list_layout> layout_p, u32 command_count = 1);
        ~F_indirect_argument_list();

        F_indirect_argument_list(const F_indirect_argument_list& x);
        F_indirect_argument_list& operator = (const F_indirect_argument_list& x);

        F_indirect_argument_list(F_indirect_argument_list&& x) noexcept;
        F_indirect_argument_list& operator = (F_indirect_argument_list&& x) noexcept;



    private:
        void allocate_data_internal();



    public:
        void reset();

    public:
        F_indirect_command_batch build();

    public:
        void draw(
            u32 command_index,
            u32 argument_index,
            u32 vertex_count,
            u32 vertex_offset
        );
        void draw_indexed(
            u32 command_index,
            u32 argument_index,
            u32 index_count,
            u32 index_offset,
            u32 vertex_offset
        );
        void draw_instanced(
            u32 command_index,
            u32 argument_index,
            u32 vertex_count_per_instance,
            u32 instance_count,
            u32 vertex_offset,
            u32 instance_offset
        );
        void draw_indexed_instanced(
            u32 command_index,
            u32 argument_index,
            u32 index_count_per_instance,
            u32 instance_count,
            u32 index_offset,
            u32 vertex_offset,
            u32 instance_offset
        );
        void dispatch(
            u32 command_index,
            u32 argument_index,
            F_vector3_u32 thread_group_counts
        );
        void input_buffer(
            u32 command_index,
            u32 argument_index,
            F_resource_gpu_virtual_address gpu_virtual_address,
            u32 size,
            u32 stride
        );
        void index_buffer(
            u32 command_index,
            u32 argument_index,
            F_resource_gpu_virtual_address gpu_virtual_address,
            u32 size,
            ED_format format
        );
        void constants(
            u32 command_index,
            u32 argument_index,
            const TG_span<u32>& values
        );
        void constant_buffer(
            u32 command_index,
            u32 argument_index,
            F_resource_gpu_virtual_address gpu_virtual_address,
            u32 size
        );
        void srv(
            u32 command_index,
            u32 argument_index,
            F_descriptor_gpu_address gpu_virtual_address
        );
        void uav(
            u32 command_index,
            u32 argument_index,
            F_descriptor_gpu_address gpu_virtual_address
        );
        void dispatch_mesh(
            u32 command_index,
            u32 argument_index,
            F_vector3_u32 thread_group_counts
        );

    public:
        NCPP_FORCE_INLINE b8 is_valid() const noexcept
        {
            return (layout_p_.is_valid() && size_);
        }
        NCPP_FORCE_INLINE b8 is_null() const noexcept
        {
            return (layout_p_.is_null() || !size_);
        }
        NCPP_FORCE_INLINE operator b8 () const noexcept
        {
            return is_valid();
        }
    };
}