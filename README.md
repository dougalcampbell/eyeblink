# eyeblink
An addressable LED animation for NeoPixels and the Particle Core/Photon. Spooky blinking eyes!

This is an LED strip animation which is supposed to resemble pairs of glowing eyes that appear, blink a few times, then disappear again. The idea is that the lights could be hidden in shrubbery, under a porch, behind a dark window, etc, to give the impression of sinister monsters watching from the shadows!

The animation is implemented as a reusable class. You can have multiple instances, each with their own settings, running simultaneously. The instances can be run on a shared NeoPixel strip instance, or you can have multiple NeoPixel instances driving multiple strips, each with their own set of eyeblink animations.

You can find more information in the [Particle Community](https://community.particle.io/t/halloween-blinky-eyes/16065) forums.

