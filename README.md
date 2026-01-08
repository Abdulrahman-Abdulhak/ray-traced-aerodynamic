raytraced-frontal-area

Headless skeleton to estimate projected frontal area using a ray-traced shadow sampling method (estimator is a TODO stub).

Build (single-command helper provided):
Windows:
  build.bat

Unix:
  ./build.sh

Run example (after build):
  bin\raytraced-frontal-area.exe --steps 10 --dt 0.1 --samples 1024 --seed 123 --rho 1.225 --cd 1.0 --wind 1 0 0

If `--mesh` is omitted, a built-in unit cube mesh is used.
