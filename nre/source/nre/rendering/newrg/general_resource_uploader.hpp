#pragma once

#include <nre/prerequisites.hpp>



namespace nre::newrg
{
    class NRE_API F_general_resource_uploader
    {
    private:
        static TK<F_general_resource_uploader> instance_p_;

    public:
        static NCPP_FORCE_INLINE TKPA_valid<F_general_resource_uploader> instance_p() { return (TKPA_valid<F_general_resource_uploader>)instance_p_; }



    private:
        TK_valid<A_command_queue> command_queue_p_;
        TU<A_command_allocator> command_allocator_p_;
        TU<A_command_list> command_list_p_;

        TU<A_fence> fence_p_;
        u64 fence_value_ = 0;

        TG_vector<TU<A_resource>> temp_resource_p_vector_;

    public:
        NCPP_FORCE_INLINE TKPA_valid<A_command_queue> command_queue_p() const noexcept { return command_queue_p_; }
        NCPP_FORCE_INLINE TK_valid<A_command_allocator> command_allocator_p() const noexcept { return NCPP_FOH_VALID(command_allocator_p_); }
        NCPP_FORCE_INLINE TK_valid<A_command_list> command_list_p() const noexcept { return NCPP_FOH_VALID(command_list_p_); }

        NCPP_FORCE_INLINE TK_valid<A_fence> fence_p() const noexcept { return NCPP_FOH_VALID(fence_p_); }
        NCPP_FORCE_INLINE u64 fence_value() const noexcept { return fence_value_; }

        NCPP_FORCE_INLINE const auto& temp_resource_p_vector() const noexcept { return temp_resource_p_vector_; }



    public:
        F_general_resource_uploader();
        ~F_general_resource_uploader();

    public:
        NCPP_OBJECT(F_general_resource_uploader);



    public:
        void upload(
            TKPA_valid<A_resource> resource_p,
            const F_initial_resource_data& initial_resource_data
        );
        void sync();
    };
}



#define NRE_NEWRG_RESOURCE_UPLOADER_COMMAND_QUEUE() nre::newrg::F_general_resource_uploader::instance_p()->command_queue_p()
#define NRE_NEWRG_RESOURCE_UPLOADER_COMMAND_ALLOCATOR() nre::newrg::F_general_resource_uploader::instance_p()->command_allocator_p()
#define NRE_NEWRG_RESOURCE_UPLOADER_COMMAND_LIST() nre::newrg::F_general_resource_uploader::instance_p()->command_list_p()