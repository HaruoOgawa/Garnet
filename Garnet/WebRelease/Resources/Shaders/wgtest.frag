struct VertexOutput {
	@builtin(position) position: vec4<f32>,
	@location(0) color: vec3<f32>,
	@location(1) uv: vec2<f32>,
};	

@fragment
fn main(in: VertexOutput) -> @location(0) vec4<f32> {
	let color = in.color;
	return vec4<f32>(color, 1.0);
}