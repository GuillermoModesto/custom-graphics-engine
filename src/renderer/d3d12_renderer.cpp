#include "d3d12_renderer.hpp"
#include <stdexcept>
using Microsoft::WRL::ComPtr;

static void ThrowIfFailed(HRESULT hr) { if (FAILED(hr)) throw std::runtime_error("D3D12 call failed"); }

bool D3D12Renderer::init(const NativeWindow& win) {
    UINT flags = 0;
#if _DEBUG
    ComPtr<ID3D12Debug> dbg;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dbg)))) dbg->EnableDebugLayer();
    flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory_)));
    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device_)));

    D3D12_COMMAND_QUEUE_DESC q{}; q.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(device_->CreateCommandQueue(&q, IID_PPV_ARGS(&queue_)));

    for (int i = 0;i < kBufferCount;i++)
        ThrowIfFailed(device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&alloc_[i])));
    ThrowIfFailed(device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, alloc_[0].Get(), nullptr, IID_PPV_ARGS(&cmd_)));
    cmd_->Close();

    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc{}; rtvDesc.NumDescriptors = kBufferCount; rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    ThrowIfFailed(device_->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap_)));
    rtvStride_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    create_swapchain(win.hwnd, win.width, win.height);

    ThrowIfFailed(device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
    fenceValue_ = 1;
    fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return true;
}

void D3D12Renderer::create_swapchain(void* windowHandle, uint32_t width, uint32_t height) {
    DXGI_SWAP_CHAIN_DESC1 sd{};
    sd.Width = width;
    sd.Height = height;
    sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferCount = kBufferCount;
    sd.SampleDesc.Count = 1;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(factory_->CreateSwapChainForHwnd(
        queue_.Get(),
        static_cast<HWND>(windowHandle),
        &sd,
        nullptr,
        nullptr,
        &swapChain1
    ));

    ThrowIfFailed(swapChain1.As(&swapchain_));

    // Create render target views (RTVs) for each back buffer
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < kBufferCount; i++) {
        ThrowIfFailed(swapchain_->GetBuffer(i, IID_PPV_ARGS(&backbuf_[i])));
        device_->CreateRenderTargetView(backbuf_[i].Get(), nullptr, rtvHandle);
        rtvHandle.ptr += SIZE_T(rtvStride_); // advance to the next descriptor
    }

    frameIndex_ = swapchain_->GetCurrentBackBufferIndex();
}

void D3D12Renderer::wait_gpu() {
    const UINT64 v = fenceValue_++;
    queue_->Signal(fence_.Get(), v);
    if (fence_->GetCompletedValue() < v) {
        fence_->SetEventOnCompletion(v, fenceEvent_);
        WaitForSingleObject(fenceEvent_, INFINITE);
    }
}

void D3D12Renderer::begin_frame() {
    alloc_[frameIndex_]->Reset();
    cmd_->Reset(alloc_[frameIndex_].Get(), nullptr);
}

void D3D12Renderer::draw() {
    D3D12_RESOURCE_BARRIER b{};
    b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    b.Transition.pResource = backbuf_[frameIndex_].Get();
    b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    b.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    cmd_->ResourceBarrier(1, &b);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
    rtv.ptr += SIZE_T(frameIndex_) * SIZE_T(rtvStride_);

    const float clear[4] = { 0.08f, 0.10f, 0.25f, 1.0f };
    cmd_->ClearRenderTargetView(rtv, clear, 0, nullptr);

    std::swap(b.Transition.StateBefore, b.Transition.StateAfter);
    cmd_->ResourceBarrier(1, &b);
}

void D3D12Renderer::end_frame() {
    cmd_->Close();
    ID3D12CommandList* lists[] = { cmd_.Get() };
    queue_->ExecuteCommandLists(1, lists);
    swapchain_->Present(1, 0);
    wait_gpu();
    frameIndex_ = swapchain_->GetCurrentBackBufferIndex();
}

