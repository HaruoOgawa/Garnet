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

@fragment
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
	let color = in.color * uTestUniform.color.rgb;
	return vec4<f32>(color, 1.0);
}