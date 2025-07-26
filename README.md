# distortion-skeleton

Distortion barebones to be filled by students.
- checkout out the repo, 
- run `git submodule update --init --recursive`
- load your ide (should support cmake)

## Hints

The work horse for the distortion is in `src/pedal/DistortionPedal.h`.

### Entry point

The function processBlock needs to be filled with the relative logic 
to process the data. It has a fixed blocksize (16 samples), this can
be exploited to write better *SIMD* optimizations.

You will find also the entry points for the parameters here.

#### Distortion

Explore concepts of distortion, 
- simple concepts like tanh functions are already cool, 
- but feel free to explore e.g. wavetable lookups, dynamic methods by measuring input signal strength.
- split handling of lower frequency and higher frequency.
- pre shaping of the tone before distortion and correcting the tone back (e.g. lower frequency attenuate with a shelving filter, and give the attenuation back after distortion)

### Strategy

Add in `src/dsp` your distortion code.

Unit-test the code in `dsp/unittests` (add files to the `CMakeLists.txt`)

Parameters that are exposed are (in order of how to add functionality):
- Distortion Type to experiment with various algorithms
- Cut to roll off the high frequencies, you could use a smple one pole low pass filter (there is one in dsp, checkout the unit-tests on how to use it)
- Preboost Low and High (the overdrive gets stronger).
   - You need to transform from db to normalized values.
   - try first using preboost low (high becomes relevant when using a crossover split or using preshaping with *shelving filters*)
- Crossover to create a split distortion (lookup *lankwitz riley filter*)

### What else

- Keep your code clean: const correctness, nicely formatted (./clang-format is included, adapt to your style)
- Use templates, algorithms, ranges and other modern stuff (this is c++20).
- Write your unit-tests, it helps...
