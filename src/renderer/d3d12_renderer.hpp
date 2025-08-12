#pragma once
#include "irenderer.hpp"
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class D3D12Renderer : public IRenderer {
public:
    bool init(const NativeWindow&) override;
    void begin_frame() override;
    void draw() override;
    void end_frame() override;

private:
    static constexpr int kBufferCount = 3;

    Microsoft::WRL::ComPtr<IDXGIFactory7> factory_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain_;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
    UINT rtvStride_ = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> backbuf_[kBufferCount];

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> alloc_[kBufferCount];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmd_;
    UINT frameIndex_ = 0;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    UINT64 fenceValue_ = 0;
    HANDLE fenceEvent_ = nullptr;

    void create_swapchain(void* hwnd, uint32_t w, uint32_t h);
    void wait_gpu();
};

