#include <nre/rendering/newrg/render_depth_pyramid_copy_from_src_binder_signature.hpp>



namespace nre::newrg
{
    TK<F_render_depth_pyramid_copy_from_src_binder_signature> F_render_depth_pyramid_copy_from_src_binder_signature::instance_p_;



    F_render_depth_pyramid_copy_from_src_binder_signature::F_render_depth_pyramid_copy_from_src_binder_signature() :
        A_binder_signature(
            {
                .param_descs = {
                    F_root_param_desc(
                        F_root_descriptor_table_desc{
                            .range_descs = {
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::SHADER_RESOURCE,
                                    .descriptor_count = 1
                                },
                                F_descriptor_range_desc {
                                    .type = ED_descriptor_range_type::UNORDERED_ACCESS,
                                    .descriptor_count = 1
                                }
                            }
                        }
                    )
                }
            },
            "NRE_NEWRG_RENDER_DEPTH_PYRAMID_COPY_FROM_SRC_BINDER_SIGNATURE"
        )
    {
        instance_p_ = NCPP_KTHIS_UNSAFE();
    }
}