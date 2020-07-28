#include "Block/BlockType.h"
#include "Block/BlockLibrary.h"

class TexIndexBuilder
{
public:
    operator TexIndices() const { return m_texIndices; }

    TexIndexBuilder& Surround(unsigned id) {m_texIndices = {id,id,id,id,id,id}; return *this;}
    TexIndexBuilder& Side(unsigned id)
    {
        m_texIndices[FaceDir::Left] = m_texIndices[FaceDir::Right] = 
        m_texIndices[FaceDir::Front] = m_texIndices[FaceDir::Back] = id; return *this;
    }
    TexIndexBuilder& Top(unsigned id) {m_texIndices[FaceDir::Top] = id; return *this;}
    TexIndexBuilder& Bottom(unsigned id) {m_texIndices[FaceDir::Bottom] = id; return *this;}
    TexIndexBuilder& Front(unsigned id) {m_texIndices[FaceDir::Front] = id; return *this;}

    TexIndices m_texIndices;
};

void RegisterBlocks()
{
    using TB = TexIndexBuilder;

    BlockLibrary::AddBlockData(BlockType::Air, {});

    // Indices go from bottom left to top right
    BlockLibrary::AddBlockData(BlockType::Dirt, {TB().Surround(0)});
    BlockLibrary::AddBlockData(BlockType::Grass,{TB().Side(1).Bottom(0).Top(2)});
    BlockLibrary::AddBlockData(BlockType::Stone,{TB().Surround(3)});
}