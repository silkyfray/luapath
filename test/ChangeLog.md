--Initial Release 0.1

--0.11 Before lab 3

-Added more maths functions to convert from the various assimp structures to glm (math funcs)
-Added assimp 4x4 matrix to SQTTransform decomposition (math funcs)
-Replaced the bone local transformations to be represented initially by a matrix 4x4 to now an SQT transform (SkinnedModel, Bone class)
-Added global timer which can also be instantiated (Timer class)
-Added keyframe animation
-Added animation queue so we can chain together animations
-added functions getValue and getTable alternatives to luapath library which return true/false instead of throwing an exception
-made some variables optional in the settings
