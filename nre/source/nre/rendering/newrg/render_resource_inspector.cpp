#include <nre/rendering/newrg/render_resource_inspector.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_render_resource_inspector::F_render_resource_inspector()
    {
    }
    F_render_resource_inspector::~F_render_resource_inspector()
    {
    }

    void F_render_resource_inspector::update_ui_internal()
    {
        auto begin_section = [](auto&& name)
        {
            ImGui::BeginChild(name, ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);
            ImGui::Text(name);
        };
        auto end_section = []()
        {
            ImGui::EndChild();
            ImGui::Dummy(ImVec2(0, 10));
        };
        auto end_section_no_gap = []()
        {
            ImGui::EndChild();
        };

        auto render_graph_p = F_render_graph::instance_p();

        auto& resource_p_owf_stack = render_graph_p->resource_p_owf_stack();
        auto resource_span = resource_p_owf_stack.item_span();

        ImGui::Begin("Render Resource Inspector");

        // transient resources
        {
            begin_section("Transient Resources");
            for(F_render_resource* resource_p : resource_span)
            {
                if(auto& allocation = resource_p->allocation())
                {
                    auto display_name = resource_p->name() + " { id: " + G_to_string(resource_p->id()).c_str() + " }";
                    begin_section(display_name.c_str());

                    auto allocator_p = allocation.allocator_p;
                    auto heap_type = allocator_p->heap_type();
                    auto heap_flags = allocator_p->heap_flags();
                    NRHI_ENUM_SWITCH(
                        heap_type,
                        NRHI_ENUM_CASE(
                            ED_resource_heap_type::GREAD_GWRITE,
                            ImGui::Text("Allocator Heap Type: GREAD_GWRITE");
                        )
                        NRHI_ENUM_CASE(
                            ED_resource_heap_type::GREAD_CWRITE,
                            ImGui::Text("Allocator Heap Type: GREAD_CWRITE");
                        )
                        NRHI_ENUM_CASE(
                            ED_resource_heap_type::CREAD_GWRITE,
                            ImGui::Text("Allocator Heap Type: CREAD_GWRITE");
                        )
                    );
                    if(flag_is_has(heap_flags, ED_resource_heap_flag::ALLOW_ONLY_BUFFERS))
                    {
                        ImGui::Text("Allocator Category: BUFFER");
                    }
                    if(flag_is_has(heap_flags, ED_resource_heap_flag::ALLOW_ONLY_RT_DS_TEXTURES))
                    {
                        ImGui::Text("Allocator Category: RT_DS_TEXTURES");
                    }
                    if(flag_is_has(heap_flags, ED_resource_heap_flag::ALLOW_ONLY_NON_RT_DS_TEXTURES))
                    {
                        ImGui::Text("Allocator Category: NON_RT_DS_TEXTURES");
                    }

                    ImGui::Text("Page Index: %d", allocation.page_index);

                    auto& allocation_placed_range = allocation.placed_range;
                    ImGui::Text("Placed Range: %d -> %d", allocation_placed_range.begin, allocation_placed_range.end);

                    end_section();
                }
            }
            end_section();
        }

        // permanent resources
        {
            begin_section("Permanent Resources");
            for(F_render_resource* resource_p : resource_span)
            {
                if(resource_p->is_permanent())
                {
                    auto display_name = resource_p->name() + " { id: " + G_to_string(resource_p->id()).c_str() + " }";
                    begin_section(display_name.c_str());

                    end_section();
                }
            }
            end_section();
        }

        ImGui::End();
    }

    void F_render_resource_inspector::enqueue_update()
    {
        F_render_graph::instance_p()->register_late_setup(
            [this]()
            {
                update_ui_internal();
            }
        );
    }
}