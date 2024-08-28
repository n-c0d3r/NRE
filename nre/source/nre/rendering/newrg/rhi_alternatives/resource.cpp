#include <nre/prerequisites.hpp>

#include <nre/rendering/newrg/general_resource_uploader.hpp>

using namespace nre;
using namespace nre::newrg;



namespace
{
    TU<A_resource> create_and_upload_resource(
        TKPA_valid<A_device> device_p,
        const F_initial_resource_data& initial_resource_data,
        const F_resource_desc& desc
    )
    {
         TU<A_resource> resource_p;

        if(desc.heap_type == ED_resource_heap_type::GREAD_CWRITE)
        {
            F_resource_desc parsed_desc = desc;
            parsed_desc.initial_state = ED_resource_state::_GENERIC_READ;

            resource_p = H_resource::create_committed(
                device_p,
                parsed_desc
            );
        }
        else
        {
            resource_p = H_resource::create_committed(
                device_p,
                desc
            );
        }

        if(initial_resource_data.size())
        {
            auto general_resource_uploader_p = F_general_resource_uploader::instance_p();
            general_resource_uploader_p->upload(
                NCPP_FOH_VALID(resource_p),
                initial_resource_data
            );
            general_resource_uploader_p->sync();
        }

        return std::move(resource_p);
    }
}



TU<A_resource> NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return create_and_upload_resource(
        device_p,
        initial_resource_data,
        desc
    );
}

U_buffer_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_buffer(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {
        create_and_upload_resource(
            device_p,
            initial_resource_data,
            desc
        )
    };
}

U_texture_1d_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_1d(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {
        create_and_upload_resource(
            device_p,
            initial_resource_data,
            desc
        )
    };
}
U_texture_2d_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_2d(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {
        create_and_upload_resource(
            device_p,
            initial_resource_data,
            desc
        )
    };
}
U_texture_3d_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_3d(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {
        create_and_upload_resource(
            device_p,
            initial_resource_data,
            desc
        )
    };
}
U_texture_2d_array_handle NRHI_DRIVER_ALTERNATIVE(nrhi, H_resource)::create_texture_2d_array(
    TKPA_valid<A_device> device_p,
    const F_initial_resource_data& initial_resource_data,
    const F_resource_desc& desc
)
{
    return {
        create_and_upload_resource(
            device_p,
            initial_resource_data,
            desc
        )
    };
}