// embed_resources - package a resources directory into a custom binary archive
// and generate a C++ .inc file with the embedded data (Qt qrc style, no zip).
//
// Usage: embed_resources <resources_dir> <out.inc>
//
// Binary format (little-endian; must match ZipManager::OpenMemoryArchive):
//   [magic "DUIR" u32][version u32][count u32]
//   count x [u32 pathLen][path UTF-8][u64 dataOffset][u64 dataLen]
//   [data blocks]
//
// The generated .inc defines:
//   static const unsigned char s_embedded_resources_data[] = { ... };
//   inline const unsigned char* GetEmbeddedResourcesData() { return s_embedded_resources_data; }
//   inline size_t GetEmbeddedResourcesSize() { return sizeof(s_embedded_resources_data); }
//
// Deterministic output: entries are sorted by path and no timestamps are stored.

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

static void WriteU32(std::vector<uint8_t>& out, uint32_t value)
{
    out.push_back(static_cast<uint8_t>(value & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

static void WriteU64(std::vector<uint8_t>& out, uint64_t value)
{
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::fprintf(stderr, "Usage: embed_resources <resources_dir> <out.inc>\n");
        return 1;
    }
    const std::string resourceDir = argv[1];
    const std::string outPath = argv[2];

    if (!fs::exists(resourceDir) || !fs::is_directory(resourceDir)) {
        std::fprintf(stderr, "embed_resources: directory not found: %s\n", resourceDir.c_str());
        return 1;
    }

    // Collect all files with their relative paths (UTF-8, '/' separators)
    struct Entry {
        std::string path;  // relative path, '/' separators
        std::vector<uint8_t> data;
    };
    std::vector<Entry> entries;
    for (const auto& dirEntry : fs::recursive_directory_iterator(resourceDir)) {
        if (!dirEntry.is_regular_file()) {
            continue;
        }
        fs::path relPath = fs::relative(dirEntry.path(), fs::path(resourceDir));
        std::string relPathA = relPath.generic_string();  // '/' separators, UTF-8 on POSIX

        // Read the file content
        std::vector<uint8_t> data;
        {
            FILE* fp = std::fopen(dirEntry.path().string().c_str(), "rb");
            if (fp == nullptr) {
                std::fprintf(stderr, "embed_resources: cannot open file: %s\n", dirEntry.path().string().c_str());
                return 1;
            }
            std::fseek(fp, 0, SEEK_END);
            long nSize = std::ftell(fp);
            std::fseek(fp, 0, SEEK_SET);
            if (nSize > 0) {
                data.resize(static_cast<size_t>(nSize));
                if (std::fread(data.data(), 1, data.size(), fp) != data.size()) {
                    std::fprintf(stderr, "embed_resources: read failed: %s\n", dirEntry.path().string().c_str());
                    std::fclose(fp);
                    return 1;
                }
            }
            std::fclose(fp);
        }
        entries.push_back({ relPathA, std::move(data) });
    }

    // Sort by path for deterministic output and binary-searchable index
    std::sort(entries.begin(), entries.end(),
              [](const Entry& a, const Entry& b) { return a.path < b.path; });

    // Assemble the binary archive
    std::vector<uint8_t> archive;
    WriteU32(archive, 0x52495544);  // "DUIR"
    WriteU32(archive, 1);           // version
    WriteU32(archive, static_cast<uint32_t>(entries.size()));

    // Index table
    for (const Entry& entry : entries) {
        WriteU32(archive, static_cast<uint32_t>(entry.path.size()));
        archive.insert(archive.end(), entry.path.begin(), entry.path.end());
        // Data offset is patched after the data blocks are laid out
        WriteU64(archive, 0);
        WriteU64(archive, entry.data.size());
    }

    // Data blocks (aligned to 8 bytes); the offsets in the index are patched
    // as each data block is laid out. NOTE: dataOffset is an ABSOLUTE offset
    // from the start of the blob (the runtime reads m_pMemoryData + dataOffset).
    const size_t indexStart = 12;  // after magic + version + count
    size_t indexPos = indexStart;
    for (size_t i = 0; i < entries.size(); ++i) {
        // Patch dataOffset: index entry layout: u32 pathLen | path | u64 offset | u64 size
        const size_t offsetFieldPos = indexPos + 4 + entries[i].path.size();
        const uint64_t dataOffset = archive.size();
        archive[offsetFieldPos + 0] = static_cast<uint8_t>(dataOffset & 0xFF);
        archive[offsetFieldPos + 1] = static_cast<uint8_t>((dataOffset >> 8) & 0xFF);
        archive[offsetFieldPos + 2] = static_cast<uint8_t>((dataOffset >> 16) & 0xFF);
        archive[offsetFieldPos + 3] = static_cast<uint8_t>((dataOffset >> 24) & 0xFF);
        archive[offsetFieldPos + 4] = static_cast<uint8_t>((dataOffset >> 32) & 0xFF);
        archive[offsetFieldPos + 5] = static_cast<uint8_t>((dataOffset >> 40) & 0xFF);
        archive[offsetFieldPos + 6] = static_cast<uint8_t>((dataOffset >> 48) & 0xFF);
        archive[offsetFieldPos + 7] = static_cast<uint8_t>((dataOffset >> 56) & 0xFF);
        indexPos += 4 + entries[i].path.size() + 16;

        // Append this entry's data and align to 8 bytes
        archive.insert(archive.end(), entries[i].data.begin(), entries[i].data.end());
        while ((archive.size() % 8) != 0) {
            archive.push_back(0);
        }
    }

    // Emit the .inc file
    FILE* fp = std::fopen(outPath.c_str(), "wb");
    if (fp == nullptr) {
        std::fprintf(stderr, "embed_resources: cannot write output: %s\n", outPath.c_str());
        return 1;
    }
    std::fprintf(fp, "///////////////////////////////////////////////////////////////////////////\n");
    std::fprintf(fp, "//   AUTO-GENERATED by embed_resources - DO NOT EDIT BY HAND\n");
    std::fprintf(fp, "//\n");
    std::fprintf(fp, "//   Source directory: %s\n", resourceDir.c_str());
    std::fprintf(fp, "//   Files: %zu, archive size: %zu bytes\n", entries.size(), archive.size());
    std::fprintf(fp, "//\n");
    std::fprintf(fp, "//   Include this file in exactly ONE .cpp of the executable, then call\n");
    std::fprintf(fp, "//   GetEmbeddedResourcesData()/GetEmbeddedResourcesSize() and pass them to\n");
    std::fprintf(fp, "//   ui::GlobalManager::Startup(ui::MemoryResParam(...)).\n");
    std::fprintf(fp, "///////////////////////////////////////////////////////////////////////////\n");
    std::fprintf(fp, "\n");
    std::fprintf(fp, "static const unsigned char s_embedded_resources_data[] = {\n");
    for (size_t i = 0; i < archive.size(); ++i) {
        if ((i % 16) == 0) {
            std::fprintf(fp, "    ");
        }
        std::fprintf(fp, "0x%02X,", archive[i]);
        if ((i % 16) == 15) {
            std::fprintf(fp, "\n");
        }
    }
    if ((archive.size() % 16) != 0) {
        std::fprintf(fp, "\n");
    }
    std::fprintf(fp, "};\n");
    std::fprintf(fp, "\n");
    std::fprintf(fp, "inline const unsigned char* GetEmbeddedResourcesData()\n");
    std::fprintf(fp, "{\n");
    std::fprintf(fp, "    return s_embedded_resources_data;\n");
    std::fprintf(fp, "}\n");
    std::fprintf(fp, "\n");
    std::fprintf(fp, "inline size_t GetEmbeddedResourcesSize()\n");
    std::fprintf(fp, "{\n");
    std::fprintf(fp, "    return sizeof(s_embedded_resources_data);\n");
    std::fprintf(fp, "}\n");
    std::fclose(fp);

    std::printf("Generated %s: %zu files, %zu bytes\n", outPath.c_str(), entries.size(), archive.size());
    return 0;
}
