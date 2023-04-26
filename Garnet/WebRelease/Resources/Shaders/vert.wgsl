struct VertexInput {
	@location(0) position: vec3<f32>,
	@location(1) color: vec3<f32>,
	@location(2) uv: vec2<f32>,
};	

struct VertexOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) color: vec3<f32>,
	@location(1) uv: vec2<f32>,
};	

struct UniformBufferObject {
	model : mat4x4<f32>,
	view : mat4x4<f32>,
	proj : mat4x4<f32>,
	mvp : mat4x4<f32>,
}

struct TestBuffer {
	MulColor : vec4<f32>,
    val0 : vec4<f32>,
    val1 : vec4<f32>,
    val2 : vec4<f32>,
}

@group(0) @binding(0) var<uniform> ubo : UniformBufferObject;
@group(0) @binding(1) var<uniform> testUBO : TestBuffer;

@vertex
fn main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = ubo.mvp * vec4<f32>(in.position, 1.0);			
	out.color = in.color * testUBO.MulColor.rgb;			
	out.uv = in.uv;			

	return out;
}