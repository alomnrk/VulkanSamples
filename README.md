# LWMetaverse

Hi!

This is LWMetaverse - Light Weight Metaverse.

Main ideas that I want to implement in this project are give the opportunity to feel the impossible, 
like falling into the Blackhole or travelling through the Fractal, keep the renderer fast
to be able to run it on mobile VR headsets like Pico and Oculus Quest, avoid long app loading resources 
like in Unity and just test different interesting rendering techniques like
Neural Rendering and Ray Marching.

For these purposes, I use Vulkan and C++.

Why not OpenGL, it's easier?
Vulkan gives more accurate control over memory and has
built-in features for VR.

Metaverse is about networking, right? But you haven't said anything about yet.
Yeah, networking is also a part of this project. As for fast transmitting data like transforms and audio - first approach will definitely use
pure udp and WebRTC protocols using Centrilized Dedicated Server model written on C++, maybe I'll also add a P2P model supporting. 
Most likely the complex server business logic (text chatting) will be separated and implemented using high-level
language GO/Python/C#.

Right now project is in really early stage, however there are some dev visual items:

BlackHole
![alt text](./readme_resources/black-hole.gif?raw=true)

Fractal RayMarching
![alt text](./readme_resources/fractal.jpg?raw=true)

PBR Material
![alt text](./readme_resources/pbr.gif?raw=true)