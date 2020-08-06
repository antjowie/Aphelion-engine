## Client side
> I make the client responsible for chunk requests since it knows which chunks it already has, instead of having to communicate this with the server. Running verification is very easy since we can just do it based on the position and server render distance

> Note: a packet can be a command. This means that it is a request. These don't have an entity ID and aren't reconciled. (for example, ChunkSpawnComponent). An example of a non-command would be the player transform.

Requesting a chunk
```
Client sends SpawnChunkComponent command
Server sends back ChunkDataComponent and ChunkModifiedComponent
```

When a chunk is modified
```
Client runs ChunkMeshBuilderSystem on the entity
```

Required systems and components
```
ChunkSpawnComponent: pos
ChunkDataComponent: pos, chunk
ChunkMeshComponent: vao

ChunkSpawnRequestSystem
ChunkMeshBuilderSystem
```

## Server side
When receiving ChunkSpawnComponent
```
if there is NOT an entity with the same pos
    Create the chunk and generate it
return the ChunkDataComponent and ChunkModifiedComponent
```