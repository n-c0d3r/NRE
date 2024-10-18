#include <nre/rendering/newrg/adapter_inspector.hpp>
#include <nre/rendering/newrg/render_foundation.hpp>
#include <nre/rendering/newrg/render_graph.hpp>
#include <nre/rendering/render_system.hpp>



namespace nre::newrg
{
    F_adapter_inspector::F_adapter_inspector()
    {
    }
    F_adapter_inspector::~F_adapter_inspector()
    {
    }

    void F_adapter_inspector::update_ui()
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

        auto adapter_p = NRE_MAIN_DEVICE()->adapter_p();

        auto& adapter_desc = adapter_p->desc();

        ImGui::Begin("Adapter Inspector");
        ImGui::Text("Description: %s", adapter_desc.description.c_str());
        ImGui::Text("Vendor Id: %u", adapter_desc.vendor_id);
        ImGui::Text("Device Id: %u", adapter_desc.device_id);
        ImGui::Text("SubSystem Id: %u", adapter_desc.subsystem_id);
        ImGui::Text("Revision: %u", adapter_desc.revision);
        ImGui::Text("Dedicated Video Memory: %lf (MiB)", f64(adapter_desc.dedicated_video_memory) / 1024.0 / 1024.0);
        ImGui::Text("Dedicated System Memory: %lf (MiB)", f64(adapter_desc.dedicated_system_memory) / 1024.0 / 1024.0);
        ImGui::Text("Shared System Memory: %lf (MiB)", f64(adapter_desc.shared_system_memory) / 1024.0 / 1024.0);
        ImGui::End();
    }
}