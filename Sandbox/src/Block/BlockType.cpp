#include "Block/BlockType.h"
#include "Block/BlockLibrary.h"

class TexIndexGenerator
{
public:
    operator TexIndices() const { return m_texIndices; }

    TexIndexGenerator& Surround(unsigned id) {m_texIndices = {id}; return *this;}
    TexIndexGenerator& Side(unsigned id)
    {
        m_texIndices[FaceDir::Left] = m_texIndices[FaceDir::Right] = 
        m_texIndices[FaceDir::Front] = m_texIndices[FaceDir::Back] = {id}; return *this;
    }
    TexIndexGenerator& Top(unsigned id) {m_texIndices[FaceDir::Top] = id; return *this;}
    TexIndexGenerator& Bottom(unsigned id) {m_texIndices[FaceDir::Bottom] = id; return *this;}
    TexIndexGenerator& Front(unsigned id) {m_texIndices[FaceDir::Front] = id; return *this;}

    TexIndices m_texIndices;
};

void RegisterBlocks()
{
    BlockLibrary::AddBlockData(BlockType::Air, {});

    BlockLibrary::AddBlockData(BlockType::Dirt, {TexIndexGenerator().Surround(2)});
    BlockLibrary::AddBlockData(BlockType::Grass,{TexIndexGenerator().Side(3).Bottom(2).Top(0)});
    BlockLibrary::AddBlockData(BlockType::Stone,{TexIndexGenerator().Surround(3)});
}