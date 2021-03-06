#pragma once
#include "Block/BlockType.h"
#include "Primitives.h"

#include <unordered_map>
#include <array>

using TexIndices = std::array<unsigned,FaceDir::Count>;

struct BlockData
{
    TexIndices texIndices;
};

/**
 * The BlockLibrary contains a map that returns the block data
 * All the blocks are the same, so we can store them in a factory/library
 * All blocks are distinguished via IDs
 */
class BlockLibrary
{
public:
    static void AddBlockData(BlockType id, BlockData&& data)
    {
        AP_CORE_ASSERT(m_library.count(id) == 0,"Already added this block");
        m_library[id] = data;
    }
    static const BlockData& GetBlockData(BlockType id)
    {
        return m_library.at(id);
    }

private:
    static std::unordered_map<BlockType,BlockData> m_library;
};