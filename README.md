# Procedural Generation

## Description

Exploring procedural generation tools for art related project.

## Getting Started

### Prerequisites

The following tools are required to build the project:

- MSYS2 GCC 13.2.0
- CMake 3.26.4
- Make 4.4.1
- Ninja 1.11.1
- OpenGL
- GLEW
- glfw3
- png

### Clone and Build

1.Clone the repository:

```bash
git clone git@github.com:remyCases/procedural_generation.git
cd procedural_generation
```

2.Build using make:

```bash
make build_release
```

### Displaying a specific element

#### Mandelbrot Fractal

```bash
make run "VAR=mandelbrot"
```

#### Canopy Fractal

```bash
make run "VAR=canopy"
```

#### Effect on image

```bash
make run "VAR=file PATH"
```
