# Problem to Address
Tensors are N-dimensional vectors/matrices. More complicated such as convolution and tensor inner/outer products are difficult to visualize and test. Here's where cymbal steps in.
## Motivating Approach
The most simply way to visualize these is to represent every element in a tensor by a unique id, then represent every tensor operation as single or composite operation against the unique id(s).
# Proof of Concept
The library `//cymbal_poc:cymbal` provides a basic API for defining a tensor and limited operations against it. See `//cymbal_poc:test` for examples.
# Specs
[[expression_specs|Expression Specs]] defines the cymbal meta language