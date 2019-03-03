# Changelog
All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to Semantic Versioning.

## Unreleased

### Changed
- BRDF
- Monte Carlo


## 0.0.1 - 2019-03-03
### Changed
- add BVH model 
- benchmark

| method              | scene               | time    |
|---------------------|---------------------|---------|
| linear search space | 487 random spheres  | 21.622s |
| BVH(height of 6)    | 487 random spheres  | 10.711s |