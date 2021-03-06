#pragma once
#include "../Type.h"

NS_MPF
NS_VSL

//位图数据
template<typename TColor>
class BitmapData
{
public:
	//创建 BitmapData 的新实例
	BitmapData(TColor* data, uint32_t width, uint32_t height, uint32_t pitch)
		:data(data), width(width), height(height), pitch(pitch), owner(false)
	{

	}

	//创建 BitmapData 的新实例
	BitmapData(uint32_t width, uint32_t height, uint32_t pitch)
		:data(nullptr), width(width), height(height), pitch(pitch), owner(true)
	{
		auto size = pitch * height;
		data = new TColor[size];
		memset(data, 0, size);
	}

	//创建 BitmapData 的新实例
	BitmapData(uint32_t width, uint32_t height)
		:data(nullptr), width(width), height(height), pitch(width), owner(true)
	{
		auto size = pitch * height;
		data = new TColor[size];
		memset(data, 0, size);
	}

	BitmapData(const BitmapData& bitmap)
		:data(bitmap.data), width(bitmap.width), height(bitmap.height), pitch(bitmap.width), owner(bitmap.owner)
	{
		if (owner && bitmap.data)
		{
			const auto size = pitch * height;
			data = new TColor[size];
			memcpy_s(data, size, bitmap.data, size);
		}
	}

	BitmapData(BitmapData&& bitmap) noexcept
		:data(bitmap.data), width(bitmap.width), height(bitmap.height), pitch(bitmap.width), owner(bitmap.owner)
	{
		bitmap.data = nullptr;
		bitmap.owner = false;
	}

	~BitmapData()
	{
		Dispose();
	}

	const BitmapData<TColor>& operator=(const BitmapData<TColor>& bitmap)
	{
		Dispose();
		data = bitmap.data;
		width = bitmap.width;
		height = bitmap.height;
		pitch = bitmap.pitch;
		owner = bitmap.owner;

		if (owner && bitmap.data)
		{
			const auto size = pitch * height;
			data = new TColor[size];
			memcpy_s(data, size, bitmap.data, size);
		}
		return *this;
	}

	const BitmapData<TColor>& operator=(BitmapData<TColor>&& bitmap) noexcept
	{
		Dispose();
		data = bitmap.data;
		width = bitmap.width;
		height = bitmap.height;
		pitch = bitmap.pitch;
		owner = bitmap.owner;
		bitmap.data = nullptr;
		bitmap.owner = false;
		return *this;
	}

	void CopyFrom(const BitmapData<byte>& source, uint32_t srcLeft = 0, uint32_t srcTop = 0, 
		uint32_t destLeft = 0, uint32_t destTop = 0)
	{
		massert(destLeft <= width);
		massert(destTop <= height);
		size_t maxX = std::min(width - destLeft, source.width - srcLeft);
		size_t maxY = std::min(height - destTop, source.height - srcTop);
		size_t srcPitch(source.pitch);
		auto ptr = data + destTop * pitch + destLeft;
		auto srcPtr = source.data + srcTop * srcPitch + srcLeft;
		for (size_t y = 0; y < maxY; y++)
		{
			for (size_t x = 0; x < maxX; x++)
			{
				ptr[x] = srcPtr[x];
			}
			srcPtr += srcPitch;
			ptr += pitch;
		}
	}

	//获取数据
	TColor* GetDataPointer() const noexcept{ return data; }
	//获取宽度（像素）
	uint32_t GetWidth() const noexcept{ return width; }
	//获取高度（像素）
	uint32_t GetHeight() const noexcept{ return height; }
	//获取Pitch（像素）
	uint32_t GetPitch() const noexcept{ return pitch; }
private:
	void Dispose() noexcept
	{
		if (owner && data)
		{
			delete[] data;
		}
		owner = false;
		data = nullptr;
	}
private:
	bool owner;
	TColor* data;
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
};

NS_ED
NS_ED