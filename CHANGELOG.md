# Changelog
All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to Semantic Versioning.

## Unreleased
### Changed
- multi-threading
- BRDF
- Monte Carlo

## 1.0.0 - 2019-04-15
### Updated
- improve BVH
- benchmark

## 0.1.0 - 2019-03-11
### Added
- add triangle

## 0.0.1 - 2019-03-03
### Changed
- add BVH model 
- benchmark

| scene              | running time (linear/BVH)|  intersection times(linear/BVH) |
|--------------------|-------------|---------------------|
| 487 random spheres | 17.955s / 12.098s | 4.0e8 / 2.4e6|
| cornell box (18 rectangle ) | 1.28s / 3.497s    | 1.5e7 / 2.8e6|