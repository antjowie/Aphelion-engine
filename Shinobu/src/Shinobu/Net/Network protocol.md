# This document is no longer applicable but will keep research for reference purposes
# Revision 2
## Core design
The core principle is to use the command pattern. The server will send command packets to the clients. These can be deferred to the game world.
Gameplay wise the server authors what data to send to the client. We won't allow the client to make such requests, the client is only capable of sending a input packet.

The justification/research of the core design can be found [here](#Explanation-of-the-core-design)

## Architecture
### Netcode protocol
The netcode protocol will handle communication between client and server. It allows connection and communication. 
- Netcode layer  
Responsible for handling communication between client and server. The netcode layer sends and received packets. It makes sure that packets are acknowledged, verifies server timeout and contains some data for us such as ping, rtt, packet loss or whatever we need. We can call this system to send packets as well as set up a callback for received packets.

### User protocol
The user protocol is what handles the players intention.
- Input layer  
Creates input data that is send to the netcode protocol. We abstract it here so that we can bind different keys to different actions. This means that only the input layer needs access to the input devices.  
    - For client side prediction, this data would also be send to the game master layer.

### Gameplay protocol
Notice how both the user and netcode protocol generate commands that act on the gameplay protocol.
- Commands  
Commands drive our game. They are callables that get called on the game world. Positions will be handled differently from events such as hitting. We differentiate between how a command is stored, predicted as well as corrected. In our game we will need 2 different kinds of commands:
    - Snapshot  
    A snapshot is linear data such as movement. This data can get interpolated and is generally unreliable. Prediction wise, we roll back if the server and client disagree. 
    - Event  
    An event is something that happens once. This can be hitting or shooting. Events are predicted and saved on the client. It then awaits verification by the server.

All state changes are commands, so all state is modified through commands (unless that data does not need to be shared). 

- Game master layer `invoker/mediator`  
Responsible for invoking commands on the game world layer. The game master stores our commands.  
Depending on our needs, there is a lot more that the game master will do:
    - To implement client side prediction, the game master will need to be able to differentiate between authoritative commands and predicted commands.
    - To implement server reconciliation, the game master needs to store older commands and be able to rollback mispredicted ones.

- Game world layer `receiver`  
Calls commands on the game world layer. The game world layer is our interface into the gameplay world.
    - To implement lag compensation, the game world needs to store older states of entities and be able to rollback to execute 

## Implementation notes
### Packet serialization
A packet contains commands. Commands are identified with a unique ID which we generate during compile time. When deserializing the command from the packet, we will create it from a factory (tho in my case, Cereal handles this. But I'd make a global linked list in which I'd register all classes).  
Commands are stored in a buffer. This can be done using a hashmap where the key is the unique ID.

## Explanation of the core design
Why do we use the command pattern? Let's first see how the game would work if it was on the client only utilizing the command pattern:
- User creates input data
- A system interprets the input data and creates commands from this (move command, mine command)
- The world is simulated and new commands are handled internally (entity got killed, block got mined)

Essentially, making the game multiplayer just means that commands have to be communicated to all clients. But multiplayer comes with it's own problems and requirements. To list a few, packets get dropped, people can have bad connection, people can be cheating. There is a lot of methods to solve these issues and I will document how they are integrated into this solution step by step.

**Authorization**  
Lets first make the game multiplayer without addressing any of these problems. The flow would be the following:
Client side:
- Client creates input data
- A system interprets the input data and creates commands from this (move command, mine command)
- The world is simulated and new commands are handled internally (entity got killed, block got mined)
- `Every command that gets created will also be send to the server`
- `The client listens for commands send by the server and will invoke it on their own world`

`Server side:`
- `Server listens for received commands`
- `Server will distribute these commands to other clients`

With this model. A client could create malicious packets and send them to the server. How can the server verify these packets? This method allows for cheating. To solve this, instead of having clients authorize their own world, the server has a world and the client synchronize with this. Clients can't directly affect this world, all simulations happen inside the server. This makes cheating a lot harder already. With this new design in mind, the flow looks as follows:
Client side:
- Client creates input data
- `Client send input data to server`
- `Client awaits commands from server`

Server side:
- `Server received input data form all clients`
- `A system interprets the input data and creates commands from this (move command, mine command)`
- `The world is simulated and new commands are handled internally (entity got killed, block got mined)`
- `The commands are send to interested clients (server gets to decide)`

This is essentially how [DOOM used to handle their netcode](https://gafferongames.com/post/what_every_programmer_needs_to_know_about_game_networking/). The issue with this however, is that the player's world doesn't update until the server sends info back. This results in the game not immediately responding to player input which is terrible for gameplay. There are however quite a few methods to solve this issue:
- Client side prediction - Excecute player input immediately before retrieving data from server.
- Server reconciliation - Only apply changes if they differ from client prediction, from there, reroll subsequent changes (perhaps?).
- Lag compensation - Rollback server simulation to client time since the player sees the world in the past.

We end up with the following flow:
Client side:
- Client creates input data
- Client sends input data to server
- Client sends input data to own world `client prediction`
- A system (gamestate administrator) interprets the input data and creates commands from this `client prediction`
- A system will listen to commands from the server
- A system (gamestate buffer) buffers all the generated commands and invokes the right commands on the game world `client prediction`. It will also roll back mispredicted commands `server reconciliation`

Server side:
- Server receives input data from all clients
- A system (gamestate administrator) interprets the input data and creates commands from this. It has to roll back the world to the time of when the packet was generated `lag compensation`
- A system (gamestate buffer) buffers the generated commands and invokes the right commands on the game world, notifying other clients as well

We need the client as well as the server to be able to simulate the world. This means that they will be executing the same code. Gameplay wise, we need a gamestate administrator to decide which commands to create, and we need a gamestate buffer to store these commands since we may have to reroll some of these (such as movement of entities (server reconciliation for client) or checking the trajectory of a projectile (lag compensation for client)).  

This brings up a problem for me. Can I use the same simulation methods for the server and client (the gamestate admin and buffer) or do I have to create two different ones for them?
Ultimately, the gamestate administrator needs to create packets based on game state. This condition is shared for both client and server. The only difference is how the commands generated from these are handles. We can use a strategy pattern for this.

Simulation loop:
- Receive commands
- Store commands
- Invoke commands
- Simulate world

# Revision 1
## Network protocol
There are two types of packets. Requests and Commands. Commands are saved in a buffer to allow for features such as
client side prediction, server reconciliation and lag prediction.
We interpolate between command states to do that. 
Requests are packets that are treated like messages. They often exchange different data. A request could be
give me a list of players. Or give me the initial world, or give me this chunk.

## Pseudocode network loop
The bare minimum to for the network protocol to work

### Client
```
Serialize last packet with user ID
Send packet on a fixed rate
```

### Server
```
Listen for packets every frame and queue them
Extract packet information of last packet
Verify packet contents
Respond or broadcast new packet
```

### Client
```
Listen for packets every frame
While new packet have arrived
    Extract packet information
    Apply packet
```

## Pseudocode joining
### Client
```
Send join packet
Await ack
```

### Server
```
Retrieves join packet
If join conditions fails (too many clients)
    Respond with disconnect packet
Else   
    Load the player into the world.
    Broadcast new entity player packet.
    Create a join packet with that unique ID.
    Return join packet to client.
```

### Client
```
If timed out
    Tell user
Else if ack is disconnected
    Tell user that they can't connect
Else if ack is joined
```

## Gameplay loop
We have one server and one client class. They both have an instance of a game world. `A game world is a structure that contains all entities and chunks`. 
What makes the server and client unique is that they decide how to update the game world. For example, the client will send packages to the server it is connected to. The server will return packages. The client will then apply these.