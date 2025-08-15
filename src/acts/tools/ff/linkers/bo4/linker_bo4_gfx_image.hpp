#pragma once
namespace fastfile::linker::bo4::image {
	bool LinkGfxImagePtr(BO4LinkContext& ctx, const char* gfximage, uint64_t* hashOut = nullptr);
}