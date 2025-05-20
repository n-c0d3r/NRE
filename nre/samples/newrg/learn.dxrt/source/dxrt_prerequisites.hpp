#pragma once

#include <nre/.hpp>

#define DXRT_DEFAULT_THREAD_GROUP_SIZE_X 64

namespace dxrt
{
    using namespace nre;

    namespace internal
    {
        class hr_exception : public std::runtime_error
        {
            inline std::string HrToString(HRESULT hr)
            {
                char s_str[64] = {};
                sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
                return std::string(s_str);
            }
        public:
            hr_exception(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
            HRESULT Error() const { return m_hr; }
        private:
            const HRESULT m_hr;
        };
    }

    inline void throw_if_failed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw internal::hr_exception(hr);
        }
    }

    inline void throw_if_failed(HRESULT hr, const wchar_t* msg)
    {
        if (FAILED(hr))
        {
            OutputDebugString(msg);
            throw internal::hr_exception(hr);
        }
    }

    inline void throw_if_false(bool value)
    {
        throw_if_failed(value ? S_OK : E_FAIL);
    }

    inline void throw_if_false(bool value, const wchar_t* msg)
    {
        throw_if_failed(value ? S_OK : E_FAIL, msg);
    }

    inline ID3D12Device5* acquire_d2d12_device_p()
    {
        ID3D12Device5* result = nullptr;
        throw_if_failed(
            NRE_MAIN_DEVICE().T_cast<F_directx12_device>()->d3d12_device_p()->QueryInterface(IID_PPV_ARGS(&result))
        );
        return result;
    }
    inline ID3D12Resource* acquire_d2d12_resource_p(const auto& resource_p)
    {
        return resource_p.template T_cast<F_directx12_resource>()->d3d12_resource_p();
    }
    inline ID3D12GraphicsCommandList4* acquire_d2d12_command_list_p(const auto& command_list_p)
    {
        ID3D12GraphicsCommandList4* result = nullptr;
        throw_if_failed(
            command_list_p.template T_cast<F_directx12_command_list>()->d3d12_command_list_p()->QueryInterface(IID_PPV_ARGS(&result))
        );
        return result;
    }
}

#define DXRT_SIZE_OF_IN_U32(obj) (((sizeof(obj) - 1) / sizeof(UINT32) + 1))