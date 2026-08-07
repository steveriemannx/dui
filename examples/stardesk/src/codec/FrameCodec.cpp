#include "FrameCodec.h"

#include <png.h>

#include <algorithm>
#include <cstring>

namespace sdk {

namespace {

// ---- PNG memory-stream callbacks ----

void PngWriteFn(png_structp png, png_bytep data, png_size_t len)
{
    std::vector<uint8_t>* buf = (std::vector<uint8_t>*)png_get_io_ptr(png);
    buf->insert(buf->end(), data, data + len);
}

void PngFlushFn(png_structp /*png*/)
{
}

struct PngReadCtx {
    const uint8_t* data;
    size_t len;
    size_t pos;
};

void PngReadFn(png_structp png, png_bytep out, png_size_t n)
{
    PngReadCtx* ctx = (PngReadCtx*)png_get_io_ptr(png);
    const size_t avail = ctx->len - ctx->pos;
    const size_t take = n < avail ? n : avail;
    std::memcpy(out, ctx->data + ctx->pos, take);
    ctx->pos += take;
}

// ---- encode/decode ----

bool PngEncodeRGBA(const uint8_t* rgba, int w, int h, int stride,
                   std::vector<uint8_t>& out)
{
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                              nullptr, nullptr);
    if (png == nullptr) {
        return false;
    }
    png_infop info = png_create_info_struct(png);
    if (info == nullptr) {
        png_destroy_write_struct(&png, nullptr);
        return false;
    }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        return false;
    }
    png_set_write_fn(png, &out, PngWriteFn, PngFlushFn);
    png_set_IHDR(png, info, (png_uint_32)w, (png_uint_32)h, 8,
                 PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_compression_level(png, 6);
    png_write_info(png, info);

    std::vector<png_bytep> rows((size_t)h);
    for (int y = 0; y < h; ++y) {
        rows[(size_t)y] = (png_bytep)(rgba + (size_t)y * stride);
    }
    png_write_image(png, rows.data());
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    return true;
}

bool PngDecodeRGBA(const uint8_t* data, size_t len,
                   std::vector<uint8_t>& rgba, int& w, int& h)
{
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                             nullptr, nullptr);
    if (png == nullptr) {
        return false;
    }
    png_infop info = png_create_info_struct(png);
    if (info == nullptr) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        return false;
    }
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        return false;
    }
    PngReadCtx ctx = { data, len, 0 };
    png_set_read_fn(png, &ctx, PngReadFn);

    png_read_info(png, info);
    png_uint_32 pw = 0, ph = 0;
    int bitDepth = 0, colorType = 0;
    png_get_IHDR(png, info, &pw, &ph, &bitDepth, &colorType, nullptr, nullptr, nullptr);
    if (pw == 0 || ph == 0 || pw > 16384 || ph > 16384) {
        png_destroy_read_struct(&png, &info, nullptr);
        return false;
    }

    // normalize to 8-bit RGBA
    if (bitDepth == 16) {
        png_set_strip_16(png);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY) {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }
    png_read_update_info(png, info);

    w = (int)pw;
    h = (int)ph;
    rgba.resize((size_t)w * h * 4);
    std::vector<png_bytep> rows((size_t)h);
    for (int y = 0; y < h; ++y) {
        rows[(size_t)y] = rgba.data() + (size_t)y * w * 4;
    }
    png_read_image(png, rows.data());
    png_read_end(png, nullptr);
    png_destroy_read_struct(&png, &info, nullptr);
    return true;
}

} // namespace

// ---------------------------------------------------------------- FrameEncoder

FrameEncoder::FrameEncoder(int quality) : m_quality(quality)
{
}

void FrameEncoder::Reset()
{
    m_prev.clear();
    m_hashes.clear();
    m_width = 0;
    m_height = 0;
}

bool FrameEncoder::EncodeFrame(const uint8_t* rgba, int width, int height,
                               std::vector<EncodedTile>& out)
{
    const bool sizeChanged = (width != m_width || height != m_height);
    if (sizeChanged) {
        m_width = width;
        m_height = height;
        m_prev.assign(rgba, rgba + (size_t)width * height * 4);
        m_tilesX = (width + kTile - 1) / kTile;
        m_tilesY = (height + kTile - 1) / kTile;
        m_hashes.assign((size_t)m_tilesX * m_tilesY, 0);
        m_tileRgba.resize((size_t)kTile * kTile * 4);

        // first frame after a size change: full frame
        EncodedTile full;
        full.x = 0;
        full.y = 0;
        full.w = width;
        full.h = height;
        if (!PngEncodeRGBA(rgba, width, height, width * 4, full.webp)) {
            return sizeChanged;
        }
        // hash the tiles of the stored frame
        for (int ty = 0; ty < m_tilesY; ++ty) {
            for (int tx = 0; tx < m_tilesX; ++tx) {
                const int x0 = tx * kTile;
                const int y0 = ty * kTile;
                const int tw = std::min(kTile, width - x0);
                const int th = std::min(kTile, height - y0);
                uint64_t hash = 1469598103934665603ull; // FNV-1a offset basis
                for (int yy = 0; yy < th; ++yy) {
                    const uint8_t* row = m_prev.data() + (size_t)((y0 + yy) * width + x0) * 4;
                    for (int xx = 0; xx < tw * 4; ++xx) {
                        hash ^= row[xx];
                        hash *= 1099511628211ull;
                    }
                }
                m_hashes[(size_t)ty * m_tilesX + tx] = hash;
            }
        }
        out.push_back(std::move(full));
        return sizeChanged;
    }

    // diff against the stored frame
    for (int ty = 0; ty < m_tilesY; ++ty) {
        for (int tx = 0; tx < m_tilesX; ++tx) {
            const int x0 = tx * kTile;
            const int y0 = ty * kTile;
            const int tw = std::min(kTile, width - x0);
            const int th = std::min(kTile, height - y0);

            uint64_t hash = 1469598103934665603ull;
            for (int yy = 0; yy < th; ++yy) {
                const uint8_t* row = rgba + (size_t)((y0 + yy) * width + x0) * 4;
                for (int xx = 0; xx < tw * 4; ++xx) {
                    hash ^= row[xx];
                    hash *= 1099511628211ull;
                }
            }
            const size_t idx = (size_t)ty * m_tilesX + tx;
            if (hash == m_hashes[idx]) {
                continue; // unchanged tile
            }
            m_hashes[idx] = hash;

            // copy the tile into a full 64x64 buffer (PNG needs a stride)
            for (int yy = 0; yy < th; ++yy) {
                std::memcpy(m_tileRgba.data() + (size_t)yy * kTile * 4,
                            rgba + (size_t)((y0 + yy) * width + x0) * 4,
                            (size_t)tw * 4);
            }
            for (int yy = th; yy < kTile; ++yy) {
                std::memset(m_tileRgba.data() + (size_t)yy * kTile * 4, 0,
                            (size_t)kTile * 4);
            }
            for (int yy = 0; yy < th; ++yy) {
                std::memset(m_tileRgba.data() + (size_t)(yy * kTile + tw) * 4, 0,
                            (size_t)(kTile - tw) * 4);
            }

            EncodedTile tile;
            tile.x = x0;
            tile.y = y0;
            tile.w = tw;
            tile.h = th;
            if (PngEncodeRGBA(m_tileRgba.data(), kTile, kTile, kTile * 4, tile.webp)) {
                out.push_back(std::move(tile));
            }
        }
    }

    // store the new frame
    std::memcpy(m_prev.data(), rgba, (size_t)width * height * 4);
    return sizeChanged;
}

bool FrameEncoder::Downscale(const uint8_t* src, int srcW, int srcH,
                             int targetW, int targetH,
                             std::vector<uint8_t>& dst, int& dstW, int& dstH)
{
    if (targetW <= 0 || targetH <= 0) {
        return false;
    }
    const double scale = std::min((double)targetW / srcW, (double)targetH / srcH);
    if (scale >= 1.0) {
        return false; // fits already
    }
    dstW = std::max(1, (int)(srcW * scale));
    dstH = std::max(1, (int)(srcH * scale));
    dst.resize((size_t)dstW * dstH * 4);

    // bilinear
    for (int y = 0; y < dstH; ++y) {
        const double sy = (double)y * srcH / dstH;
        const int y0 = (int)sy;
        const int y1 = std::min(srcH - 1, y0 + 1);
        const double fy = sy - y0;
        for (int x = 0; x < dstW; ++x) {
            const double sx = (double)x * srcW / dstW;
            const int x0 = (int)sx;
            const int x1 = std::min(srcW - 1, x0 + 1);
            const double fx = sx - x0;

            const uint8_t* p00 = src + (size_t)(y0 * srcW + x0) * 4;
            const uint8_t* p01 = src + (size_t)(y0 * srcW + x1) * 4;
            const uint8_t* p10 = src + (size_t)(y1 * srcW + x0) * 4;
            const uint8_t* p11 = src + (size_t)(y1 * srcW + x1) * 4;
            uint8_t* o = dst.data() + (size_t)(y * dstW + x) * 4;
            for (int c = 0; c < 4; ++c) {
                const double top = p00[c] * (1 - fx) + p01[c] * fx;
                const double bot = p10[c] * (1 - fx) + p11[c] * fx;
                o[c] = (uint8_t)(top * (1 - fy) + bot * fy);
            }
        }
    }
    return true;
}

// ---------------------------------------------------------------- FrameDecoder

bool FrameDecoder::DecodeTileInto(const uint8_t* png, size_t len,
                                  uint8_t* dst, int dstW, int dstH,
                                  int x, int y)
{
    std::vector<uint8_t> decoded;
    int w = 0, h = 0;
    if (!PngDecodeRGBA(png, len, decoded, w, h)) {
        return false;
    }
    // blit into the frame (may be partially off-screen for edge tiles)
    for (int yy = 0; yy < h; ++yy) {
        const int dstY = y + yy;
        if (dstY < 0 || dstY >= dstH) {
            continue;
        }
        for (int xx = 0; xx < w; ++xx) {
            const int dstX = x + xx;
            if (dstX < 0 || dstX >= dstW) {
                continue;
            }
            std::memcpy(dst + (size_t)(dstY * dstW + dstX) * 4,
                        decoded.data() + (size_t)(yy * w + xx) * 4, 4);
        }
    }
    return true;
}

} // namespace sdk
