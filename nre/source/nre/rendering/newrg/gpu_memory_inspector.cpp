#include <nre/rendering/newrg/gpu_memory_inspector.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>



namespace nre::newrg
{
    F_gpu_memory_inspector::F_gpu_memory_inspector()
    {
    }
    F_gpu_memory_inspector::~F_gpu_memory_inspector()
    {
    }

    void F_gpu_memory_inspector::update_ui_internal()
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

        ImGui::Begin("GPU Memory Inspector");

        // overall
        {
            begin_section("Overall");
            F_gpu_memory_info gpu_memory_info = NRE_MAIN_DEVICE()->adapter_p()->gpu_memory_info();
            ImGui::Text("Budget: %lf (MiB)", f64(gpu_memory_info.budget) / 1024.0 / 1024.0);
            ImGui::Text("Current Usage: %lf (MiB)", f64(gpu_memory_info.current_usage) / 1024.0 / 1024.0);
            end_section();

            begin_section("Render Graph Resource Allocators");
            auto& resource_allocators = render_graph_p->resource_allocators();
            for(auto& resource_allocator : resource_allocators)
            {
#ifdef NRHI_ENABLE_DRIVER_DEBUGGER
                begin_section(resource_allocator.name().c_str());
#else
                begin_section("Unnamed");
#endif
                auto& pages = resource_allocator.pages();
                ImGui::Text("Page Capacity: %lf (MiB)", f64(resource_allocator.page_capacity()) / 1024.0 / 1024.0);
                ImGui::Text("Pages: %d", resource_allocator.pages().size());
                ImGui::Text("Current Page Budget: %lf (MiB)", f64(resource_allocator.pages().size() * resource_allocator.page_capacity()) / 1024.0 / 1024.0);\
                end_section();
            }
            end_section();
        }

        ImGui::End();
    }

    void F_gpu_memory_inspector::enqueue_update()
    {
        F_render_graph::instance_p()->register_late_setup(
            [this]()
            {
                update_ui_internal();
            }
        );
    }
}