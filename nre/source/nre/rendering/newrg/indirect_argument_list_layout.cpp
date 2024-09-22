#include <nre/rendering/newrg/indirect_argument_list_layout.hpp>
#include <nre/rendering/newrg/indirect_argument_list.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    F_indirect_argument_list_layout::F_indirect_argument_list_layout(
        const F_command_signature_desc& desc,
        TKPA_valid<A_root_signature> root_signature_p
    )
    {
        F_command_signature_desc parsed_desc = desc;

        table_.slots.resize(parsed_desc.indirect_argument_descs.size());
        for(u32 i = 0; i < table_.slots.size(); ++i)
        {
            auto& slot = table_.slots[i];
            auto& argument_desc = parsed_desc.indirect_argument_descs[i];

            table_.alignment = align_address(
                table_.alignment,
                H_indirect_argument::alignment(argument_desc.type)
            );

            slot.offset = align_address(
                table_.stride,
                table_.alignment
            );
            slot.size = H_indirect_argument::size(argument_desc);

            table_.stride = slot.offset + slot.size;
        }
        table_.stride = align_address(table_.alignment);

        parsed_desc.stride = table_.stride;

        command_signature_p_ = H_command_signature::create(
            NRE_MAIN_DEVICE(),
            parsed_desc,
            root_signature_p
        );
    }
    F_indirect_argument_list_layout::~F_indirect_argument_list_layout()
    {
    }
}
