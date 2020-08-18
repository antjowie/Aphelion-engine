Since this is the first time that I integrate PhysX, I'll write down some API usage notes here

# Startup and Shutdown
Create the following systems:
```
Foundation
Physics
```


Optional systems
```
Cooking
Extensions
```
The Foundation is a system that all other systems/modules need. It is passed in their respective PxCreateXXX function
When shutting down, release in reversed init order with the `release` function.

# Geometry
