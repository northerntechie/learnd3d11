#include "TriangleApplication.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

using Position = DirectX::XMFLOAT3;
using Color = DirectX::XMFLOAT3;

struct VertexPositionColor
{
    Position position;
    Color color;
};

TriangleApplication::TriangleApplication(const std::string& title)
	: Application(title) {}

bool TriangleApplication::Initialize()
{
	return false;
}

bool TriangleApplication::Load()
{
	ComPtr<ID3DBlob> vertexShaderBlob = nullptr;

	_vertexShader = CreateVertexShader(L"Assets/Shaders/Main.vs.hlsl", vertexShaderBlob);
	if (_vertexShader == nullptr)
	{
		return false;
	}

	_pixelShader = CreatePixelShader(L"Assets/Shaders/Main.ps.hlsl");
	if (_pixelShader == nullptr)
	{
		return false;
	}

	constexpr D3D11_INPUT_ELEMENT_DESC vertexInputLayoutInfo[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexPositionColor, position),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			offsetof(VertexPositionColor, color),
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
	};

	if (FAILED(_device->CreateInputLayout(
		vertexInputLayoutInfo,
		_countof(vertexInputLayoutInfo),
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		&_vertexLayout)))
	{
		std::cout << "D3D11: Failed to create default vertex input layout\n";
		return false;
	}
}

void TriangleApplication::Render()
{
}

void TriangleApplication::Update()
{
	Application::Update();
}

bool TriangleApplication::CompileShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& profile, ComPtr<ID3DBlob>& shaderBlob) const
{
	constexpr UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	ComPtr<ID3DBlob> tempShaderBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	if (FAILED(D3DCompileFromFile(
		fileName.data(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.data(),
		profile.data(),
		compileFlags,
		0,
		&tempShaderBlob,
		&errorBlob)))
	{
		std::cout << "D3D11: Failed to read shader from file\n";
		if (errorBlob != nullptr)
		{
			std::cout << "D3D11: With message: " << 
			static_cast<const char*>(errorBlob->GetBufferPointer()) << "\n";
		}

		return false;
	}

	shaderBlob = std::move(tempShaderBlob);
	return true;
}

ComPtr<ID3D11VertexShader> TriangleApplication::CreateVertexShader(const std::wstring& fileName, ComPtr<ID3DBlob>& vertexShaderBlob) const
{
	if (!CompileShader(fileName, "Main", "vs_5_0", vertexShaderBlob))
	{
		return nullptr;
	}

	ComPtr<ID3D11VertexShader> vertexShader;
	if (FAILED(_device->CreateVertexShader(
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize(),
		nullptr,
		&vertexShader)))
	{
		std::cout << "D3D11: Failed to compile vertex shader\n";
		return nullptr;
	}

	return vertexShader;
}

ComPtr<ID3D11PixelShader> TriangleApplication::CreatePixelShader(const std::wstring& fileName) const
{
	ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
	if (!CompileShader(fileName, "Main", "ps_5_0", pixelShaderBlob))
	{
		return nullptr;
	}

	ComPtr<ID3D11PixelShader> pixelShader;
	if (FAILED(_device->CreatePixelShader(
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize(),
		nullptr,
		&pixelShader)))
	{
		std::cout << "D3D11: Failed to compile pixel shader\n";
		return nullptr;
	}

	return pixelShader;
}
