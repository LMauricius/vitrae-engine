# Task pipelines

## What are pipelines
Pipelines are series of tasks that generate results. They are used to render scenes, shade pixels and (in the future, possibly) generate resources. They consist of modular tasks that can be reused as different subtasks or in different pipelines.

## Divide and conquer
Pipelines consist of tasks. Each task is an asset in itself and can be reused in multiple pipelines.
Which tasks will be used to produce a final results depends on Methods used.

## Components

### Tasks
Tasks have inputs and outputs (results).

### Methods
Methods are collections of tasks.
Since multiple Tasks can give the same result, we need a way to separate them.
A method is a way of providing a single Task per desired result, meaning no two Tasks can give the same result.

A Method can have any number of fallback Methods specified,
which are searched through if no Task for the desired result is directly found in this Method.
Multiple fallback Methods can provide a Task for the same result, but only the first fitting Task is returned.

### Pipelines
Pipelines are constructed by specifying a preferred Method an desired results.
Then the pipeline selects needed tasks for generating the desired results and builds a list of PipeItems,
along with its own required inputs and local variables.
A PipeItem is defined by its Task, and mappings between variables local to the Pipeline and Task inputs/outputs.

## Examples
Currently there are two kinds of pipelines:

- Shading pipelines
- Rendering pipelines

Both are based on `AbstractTask` pipelines, and support properties of similar types, but differ on their basic task type. 
 
- Shading:
    - Basic task: ShaderTask
    - Primitive tasks:
        - ShaderCode
        - ShaderLoop
    - Value getters:
        - ShaderConstant
    - End results:
        - `fragment_shade` - The color of a pixel in SurfaceShader
        - `computed_data` - Output data of a ComputeShader
- Rendering:
    - Basic task: HostTask
    - Primitive tasks:
        - RenderView
        - ExecuteFunction
        - ExecuteComputeShader
    - Value getters:
        - HostConstant
    - End results:
        - `display` - A token result representing a finished image.