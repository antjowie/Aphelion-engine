#include "Block/BlockType.h"
#include "Block/BlockLibrary.h"

class TexIndexGenerator
{
public:
    operator TexIndices() const { return m_texIndices; }

    TexIndexGenerator& Surround(unsigned id) {m_texIndices = {id,id,id,id,id,id}; return *this;}
    TexIndexGenerator& Side(unsigned id)
    {
        m_texIndices[FaceDir::Left] = m_texIndices[FaceDir::Right] = 
        m_texIndices[FaceDir::Front] = m_texIndices[FaceDir::Back] = id; return *this;
    }
    TexIndexGenerator& Top(unsigned id) {m_texIndices[FaceDir::Top] = id; return *this;}
    TexIndexGenerator& Bottom(unsigned id) {m_texIndices[FaceDir::Bottom] = id; return *this;}
    TexIndexGenerator& Front(unsigned id) {m_texIndices[FaceDir::Front] = id; return *this;}

    TexIndices m_texIndices;
};

void RegisterBlocks()
{
    BlockLibrary::AddBlockData(BlockType::Air, {});

    // Goes from bottom left to top right
    BlockLibrary::AddBlockData(BlockType::Dirt, {TexIndexGenerator().Surround(0)});
    BlockLibrary::AddBlockData(BlockType::Grass,{TexIndexGenerator().Side(1).Bottom(0).Top(2)});
    BlockLibrary::AddBlockData(BlockType::Stone,{TexIndexGenerator().Surround(3)});
}