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

struct TestBuffer {
	m0 : mat4x4<f32>,
	m1 : mat4x4<f32>,
	m2 : mat4x4<f32>,
	m3 : mat4x4<f32>,
}

@group(0) @binding(0) var<uniform> val : TestBuffer;
@group(0) @binding(1) var<uniform> mvp : mat4x4<f32>;

@vertex
fn main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = mvp * vec4<f32>(in.position, 1.0);			
	out.color = in.color;			
	out.uv = in.uv;			

	return out;
}