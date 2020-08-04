## Client side
> I make the client responsible for chunk requests since it knows which chunks it already has, instead of having to communicate this with the server. Running verification is very easy since we can just do it based on the position and server render distance


Generating a chunk
```
Client spawn entity with ChunkSpawnComponent
For all ChunkSpawnComponent request server for the chunk
Server sends back ChunkServerAckComponent
For all ChunkSpawnComponent and ChunkServerAckComponent. Build ChunkMesh and delete first 2 components
```
 Required systems and components
```
ChunkSpawnComponent: pos
ChunkServerAckComponent
ChunkDataComponent: pos, chunk

ChunkSpawnRequestSystem
ChunkMeshBuilderSystem
```

## Server side
When receiving ChunkSpawnComponent
```
If data at chunk pos is empty
    Generate chunk
Send Chunk Data to player
```