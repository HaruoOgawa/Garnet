struct gl_PerVertex {
    @builtin(position) gl_Position: vec4<f32>,
    gl_PointSize: f32,
    gl_ClipDistance: array<f32,1u>,
    gl_CullDistance: array<f32,1u>,
}

struct VertexOutput {
    @builtin(position) gl_Position: vec4<f32>,
    @location(0) member: vec2<f32>,
}

var<private> perVertexStruct: gl_PerVertex = gl_PerVertex(vec4<f32>(0.0, 0.0, 0.0, 1.0), 1.0, array<f32,1u>(0.0), array<f32,1u>(0.0));
var<private> inPosition_1: vec3<f32>;
var<private> fragTexCoord: vec2<f32>;
var<private> inTexcoord_1: vec2<f32>;
var<private> inNormal_1: vec3<f32>;
var<private> inTangent_1: vec4<f32>;

fn main_1() {
    let _e13 = inPosition_1;
    perVertexStruct.gl_Position = vec4<f32>(_e13.x, _e13.y, _e13.z, 1.0);
    let _e19 = inTexcoord_1;
    fragTexCoord = _e19;
    return;
}

@vertex 
fn main(@location(0) inPosition: vec3<f32>, @location(2) inTexcoord: vec2<f32>, @location(1) inNormal: vec3<f32>, @location(3) inTangent: vec4<f32>) -> VertexOutput {
    inPosition_1 = inPosition;
    inTexcoord_1 = inTexcoord;
    inNormal_1 = inNormal;
    inTangent_1 = inTangent;
    main_1();
    let _e12 = perVertexStruct.gl_Position.y;
    perVertexStruct.gl_Position.y = -(_e12);
    let _e14 = perVertexStruct.gl_Position;
    let _e15 = fragTexCoord;
    return VertexOutput(_e14, _e15);
}
