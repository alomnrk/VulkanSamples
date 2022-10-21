# LWMetaverse

Hi!

This is LWMetaverse - Light Weight Metaverse.

Main ideas that I want to develop in this project are give the oppurtunity to feel the impossible, 
like falling into the blackhole or travelling through the fractal, keep the renderer fast
to be able ran it on mobile VR headsets like Pico and Oculus Quest, avoid long app loading resources 
like in Unity and just test different interesting rendering techniques like
Neural Rendering and Ray Marching.

For these purposes, I use Vulkan and C++.

Why not OpenGL, it's easier?
Vulkan gives more accurate control about memory ang has integrated
features for VR.

Metaverse is about networking, right? But you haven't said anything about net.
Yeah, networking is also a part of this project. As for fast transmitting data like transforms - first approach will defenetly use
pure tcp/udp protocols, WebRTC for audio and Centrilized Dedicated Server written on C++, maybe I'll also add P2P model supporting. 
Most likely the complex business logic (text chatting) will be separated and implemented using high-level
language GO/Python/C#.

Right now project is in really early stage, however there are some dev visual items:

# BlackHole
![alt text](./readme_resources/black-hole.gif?raw=true)

# Fractal RayMarching
![alt text](./readme_resources/fractal.jpg?raw=true)

# PBR Material
![alt text](./readme_resources/pbr.gif?raw=true)
