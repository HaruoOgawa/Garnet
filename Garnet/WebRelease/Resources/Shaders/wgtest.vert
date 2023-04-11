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
	
struct TestUniform {
	color: vec4<f32>,
	time: f32,
	pad0: f32,
	pad1: f32,
	pad2: f32,
	mvp : mat4x4<f32>,
};

@group(0) @binding(0) var<uniform> uTestUniform: TestUniform;
//@group(0) @binding(1) var<uniform> uTestXOff: f32;

@vertex
fn main(in: VertexInput) -> VertexOutput {
	var offset = vec3<f32>(0.0, sin(uTestUniform.time), 0.0);

	var out: VertexOutput;
	out.position = uTestUniform.mvp * vec4<f32>(in.position + offset, 1.0);			
	out.color = in.color;			
	out.uv = in.uv;			

	return out;
}