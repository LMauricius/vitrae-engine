# Task pipelines

## What are pipelines
Pipelines are complex series of tasks that generate data. They are used to render scenes, shade pixels and (in the future, possibly) generate resources. They consist of modular tasks that can be reused as different subtasks or in different pipelines. Tasks each have 

## Divide and conquer
Pipelines are divided into subtasks. Each task is an asset in itself and can be reused in multiple pipelines.

## Examples
Currently there are two kinds of pipelines:

- Shading pipelines
- Rendering pipelines

Both are based on `AbstractTask` pipelines, and support properties of similar types, but differ on their basic task type. 

- Shading:
    - basic task: ShaderFunction
    - Value getters:
        - ShaderConstant
- Rendering:
    - basic task: CpuTask
    - Value getters:
        - TextureGetter

## Tasks
Tasks inherit from an `AbstractTask` class. Each pipeline type has its "basic task",
which can be put as a `BasicTask` template parameter for some generic reusable tasks.

There are 3 types of generic tasks:

- `GenericGroup`
- `GenericSwitch`
- `GenericFilter`

Each of them can be extracted with given arguments into a linear series of their `BasicTask`s.
Basic tasks don't have to be a single final class, but can be inherited from and extended.