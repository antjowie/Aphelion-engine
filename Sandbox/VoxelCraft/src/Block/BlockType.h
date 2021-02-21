#pragma once

/**
 * This file defines the block types and their properties
 */

enum class BlockType : uint8_t
{
    Air,

    Dirt,
    Grass,
    Stone,
};

void RegisterBlocks();