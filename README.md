# Vocal_Analysis

# Build
```
cmake -S . -B build
cmake --build build
```

# Test
```
cd build
ctest
```

# Code

Code Convention: https://juce.com/feature/coding-standards/

# Git

## Branching
Branch naming: category/issue-##/description

Example: feature/issue-9/update-readme

Categories
* **feature** is for adding, refactoring or removing a feature
* **bugfix** is for fixing a bug
* **hotfix** is for changing code with a temporary solution and/or without following the usual process (usually because of an emergency)
* **test** is for experimenting outside of an issue/ticket

https://dev.to/varbsan/a-simplified-convention-for-naming-branches-and-commits-in-git-il4

## Commit message
Use the issue title or a clear text on what changed.
Use newline space and then add Fixing #issueNumber

Example:
```
Issue description/change text

Fixing #9
```