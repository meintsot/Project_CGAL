# Eliminating Obtuse Triangles in Delaunay Triangulations While Minimizing Steiner Points

## Table of Contents

- [Eliminating Obtuse Triangles in Delaunay Triangulations While Minimizing Steiner Points](#eliminating-obtuse-triangles-in-delaunay-triangulations-while-minimizing-steiner-points)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Delaunay Triangulation](#delaunay-triangulation)
    - [Definition](#definition)
    - [Properties](#properties)
    - [Construction Methods](#construction-methods)
  - [Problem of Obtuse Triangles](#problem-of-obtuse-triangles)
    - [Why Eliminate Obtuse Triangles?](#why-eliminate-obtuse-triangles)
  - [Valid Edge Algorithm](#valid-edge-algorithm)
    - [Overview](#overview)
    - [Edge Flipping](#edge-flipping)
      - [Criteria for Edge Flipping:](#criteria-for-edge-flipping)
    - [Algorithm Steps](#algorithm-steps)
  - [Minimizing Steiner Points](#minimizing-steiner-points)
    - [What Are Steiner Points?](#what-are-steiner-points)
    - [When to Add Steiner Points](#when-to-add-steiner-points)
  - [Algorithm for Eliminating Obtuse Triangles](#algorithm-for-eliminating-obtuse-triangles)
    - [Step-by-Step Explanation](#step-by-step-explanation)
      - [1. Initial Triangulation](#1-initial-triangulation)
      - [2. Valid Edge Algorithm (Edge Flipping)](#2-valid-edge-algorithm-edge-flipping)
      - [3. Check for Remaining Obtuse Triangles](#3-check-for-remaining-obtuse-triangles)
      - [4. Steiner Point Insertion (If Necessary)](#4-steiner-point-insertion-if-necessary)
      - [5. Finalization](#5-finalization)
  - [Implementation Considerations](#implementation-considerations)
    - [Handling Constraints](#handling-constraints)
    - [Using CGAL](#using-cgal)
  - [Conclusion](#conclusion)
  - [References](#references)

---

## Introduction

In computational geometry and mesh generation, creating high-quality triangulations is essential for various applications such as finite element analysis, computer graphics, and geographic information systems. One of the goals in mesh generation is to eliminate obtuse triangles to improve numerical stability and accuracy while minimizing the introduction of additional points, known as **Steiner points**.

This document provides a detailed explanation of the steps involved in generating a Delaunay triangulation, eliminating obtuse triangles using the **Valid Edge Algorithm**, and introducing Steiner points only when necessary.

---

## Delaunay Triangulation

### Definition

A **Delaunay triangulation** for a set of points in a plane is a triangulation such that no point is inside the circumcircle of any triangle in the triangulation. In other words, the circumcircle of each triangle contains only the three vertices of that triangle and no other points from the set.

### Properties

- **Maximizes the Minimum Angle**: Delaunay triangulations tend to avoid skinny triangles by maximizing the minimum angle among all the angles of the triangles in the triangulation.
- **Uniqueness**: For a set of points in general position (no four points are co-circular), the Delaunay triangulation is unique.
- **Empty Circumcircle Property**: The circumcircle of any triangle in the Delaunay triangulation contains no other points from the set.

### Construction Methods

Several algorithms can construct a Delaunay triangulation:

- **Incremental Insertion**: Points are added one at a time, updating the triangulation at each step.
- **Divide and Conquer**: The point set is divided into smaller subsets, each triangulated separately, and then merged.
- **Sweep Line Algorithm**: A line sweeps across the plane, and the triangulation is built incrementally.
- **Bowyer-Watson Algorithm**: A method based on inserting points and re-triangulating affected regions.

---

## Problem of Obtuse Triangles

### Why Eliminate Obtuse Triangles?

- **Numerical Stability**: Obtuse triangles can cause numerical instability in simulations, such as finite element methods.
- **Mesh Quality**: High-quality meshes with angles close to equilateral triangles are preferred for accurate interpolation and computation.
- **Convergence**: In iterative methods, better-shaped elements can lead to faster convergence.

However, Delaunay triangulations do not guarantee the absence of obtuse triangles, especially in constrained scenarios or with specific point distributions.

---

## Valid Edge Algorithm

### Overview

The **Valid Edge Algorithm** aims to eliminate obtuse triangles by adjusting the edges in the triangulation without introducing new points (Steiner points). It does this by performing local modifications, primarily through **edge flipping**, to improve the mesh quality.

### Edge Flipping

**Edge flipping** is a local operation where an edge shared by two adjacent triangles is replaced with the other diagonal of the quadrilateral formed by those two triangles.

#### Criteria for Edge Flipping:

- **Convex Quadrilateral**: The quadrilateral formed by the two triangles must be convex.
- **Improvement in Mesh Quality**: Flipping should result in an increase in the minimum angle or eliminate an obtuse angle.
- **Non-Constrained Edges**: Only edges that are not part of the original constraints can be flipped.

### Algorithm Steps

1. **Initialization**: Start with an initial Delaunay triangulation of the given point set and constraints.

2. **Identify Edges for Flipping**:
   - Traverse all non-constrained edges in the triangulation.
   - For each edge, check if flipping it would improve the mesh quality.

3. **Edge Flipping Process**:
   - **Compute Angles**: Calculate the angles of the triangles before and after flipping.
   - **Decision**: Flip the edge if it increases the minimum angle or eliminates an obtuse angle.
   - **Update Triangulation**: Perform the flip and update the triangulation data structures.

4. **Iterate Until Convergence**:
   - Repeat steps 2 and 3 until no further improvements can be made through edge flipping.

5. **Termination**:
   - The algorithm terminates when all eligible edges have been considered and no more flips can improve the mesh quality.

---

## Minimizing Steiner Points

### What Are Steiner Points?

**Steiner points** are additional points not part of the original input that are inserted into the mesh to improve its quality. They can help eliminate obtuse triangles and improve element shapes but increase the total number of points and triangles.

### When to Add Steiner Points

- **Edge Flipping Insufficient**: If obtuse triangles cannot be eliminated through edge flipping alone, Steiner points may be necessary.
- **Mesh Quality Criteria**: When the mesh does not meet certain quality criteria (e.g., minimum angle thresholds), adding Steiner points can help achieve these goals.

---

## Algorithm for Eliminating Obtuse Triangles

### Step-by-Step Explanation

#### 1. Initial Triangulation

- **Input**: A set of points and constraints (edges that must be preserved).
- **Action**: Construct an initial **Constrained Delaunay Triangulation (CDT)** using the given points and constraints.
- **Tools**: Use a computational geometry library like CGAL for efficient triangulation.

#### 2. Valid Edge Algorithm (Edge Flipping)

- **Objective**: Improve mesh quality by eliminating obtuse triangles without adding new points.
- **Process**:
  - **Traverse Non-Constrained Edges**:
    - Iterate over all edges that are not constrained.
  - **Evaluate Edge Flip**:
    - For each eligible edge, calculate the minimum angle in the adjacent triangles before and after a hypothetical flip.
    - **Flip Criterion**: Flip the edge if it increases the minimum angle or eliminates an obtuse angle.
  - **Perform Flip**:
    - Update the triangulation by flipping the edge if it meets the criterion.
  - **Repeat**:
    - Continue iterating over edges until no more beneficial flips are possible.
- **Constraints Handling**:
  - Ensure that constrained edges are not flipped to preserve the input geometry.

#### 3. Check for Remaining Obtuse Triangles

- **Action**: After edge flipping, check if any obtuse triangles still exist in the triangulation.
- **Process**:
  - Iterate over all triangles in the triangulation.
  - Use geometric calculations to determine if any triangle has an angle greater than 90 degrees.

#### 4. Steiner Point Insertion (If Necessary)

- **Objective**: Eliminate remaining obtuse triangles by adding Steiner points.
- **Process**:
  - **Identify Problematic Triangles**:
    - Locate triangles that are still obtuse after edge flipping.
  - **Determine Longest Edge**:
    - For each obtuse triangle, find the longest edge.
  - **Compute Midpoint**:
    - Calculate the midpoint of the longest edge.
  - **Insert Steiner Point**:
    - Add the midpoint as a new point in the triangulation.
    - Update the triangulation to include the new point.
  - **Update Constraints**:
    - Ensure that new edges formed do not violate any constraints.
- **Iteration**:
  - Repeat the process of checking for obtuse triangles and inserting Steiner points until all triangles are acute or meet the desired angle criteria.

#### 5. Finalization

- **Output**: A triangulation where all triangles are acute (no obtuse angles), and the number of Steiner points is minimized.
- **Data Preparation**:
  - Collect the final set of points and triangles.
  - Prepare data structures for further processing or output.

---

## Implementation Considerations

### Handling Constraints

- **Constrained Edges**: Edges that represent boundaries or must be preserved cannot be flipped or altered.
- **Constraint Preservation**: Ensure that all operations respect the constraints to maintain the validity of the input geometry.
- **Data Structures**: Use appropriate data structures to keep track of constrained edges and prevent invalid modifications.

### Using CGAL

- **CGAL Library**: The Computational Geometry Algorithms Library (CGAL) provides robust implementations for triangulations and mesh operations.
- **Constrained Delaunay Triangulation**:
  - Use `CGAL::Constrained_Delaunay_triangulation_2` for triangulations with constraints.
- **Edge Flipping**:
  - Direct edge flipping may not be available for CDTs in CGAL due to constraints.
  - Use mesh optimization functions provided by CGAL, such as `refine_Delaunay_mesh_2`, which internally handle flipping and point insertion.
- **Mesh Criteria**:
  - Set criteria for mesh refinement to control the minimum angle and other quality parameters.
- **Steiner Point Management**:
  - CGAL's mesh refinement functions can manage Steiner points efficiently while adhering to quality criteria.

---

## Conclusion

Eliminating obtuse triangles in a triangulation while minimizing the number of Steiner points is a balance between mesh quality and complexity. The **Valid Edge Algorithm** provides a method to improve the mesh by adjusting existing edges through flipping, without adding new points. When edge flipping is insufficient due to constraints or geometric limitations, **Steiner points** are introduced as a last resort to meet the desired mesh quality.

By carefully applying these techniques and leveraging computational geometry tools like CGAL, it is possible to generate high-quality triangulations suitable for a variety of applications, ensuring numerical stability and computational efficiency.

---

## References

- **Computational Geometry Algorithms Library (CGAL)**: [https://www.cgal.org/](https://www.cgal.org/)
- **Mesh Generation and Optimization**:
  - Jonathan Richard Shewchuk. "Delaunay Refinement Mesh Generation". Ph.D. thesis, Carnegie Mellon University, 1997.
  - O. Devillers, S. Pion, M. Teillaud. "Walking in a Triangulation". International Journal of Foundations of Computer Science, 2001.
- **Edge Flipping and Mesh Quality**:
  - L. P. Chew. "Guaranteed-Quality Mesh Generation for Curved Surfaces". Proceedings of the Ninth Annual Symposium on Computational Geometry, 1993.

---