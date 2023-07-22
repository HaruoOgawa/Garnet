struct UniformBufferObject {
  /* @offset(0) */
  UseBlur : i32,
  /* @offset(4) */
  KernelSize : i32,
  /* @offset(8) */
  Direction : vec2<f32>,
}

struct strided_arr {
  @size(16)
  el : f32,
}

struct GaussianKernelBuffer {
  /* @offset(0) */
  Kernal : array<strided_arr, 32u>,
}

@group(0) @binding(0) var<uniform> ubo : UniformBufferObject;

@group(0) @binding(2) var SrcTex : texture_2d<f32>;

@group(0) @binding(3) var SamplerSrcTex : sampler;

var<private> f_UV : vec2<f32>;

@group(0) @binding(1) var<uniform> kbo : GaussianKernelBuffer;

var<private> outColor : vec4<f32>;

fn main_1() {
  var col : vec3<f32>;
  var dir : vec2<f32>;
  var halfSize : i32;
  var i : i32;
  col = vec3<f32>(0.0f, 0.0f, 0.0f);
  let x_22 : vec2<f32> = ubo.Direction;
  dir = x_22;
  let x_28 : i32 = ubo.KernelSize;
  halfSize = ((x_28 - 1i) / 2i);
  let x_33 : i32 = ubo.UseBlur;
  if ((x_33 != 0i)) {
    i = 0i;
    loop {
      let x_44 : i32 = i;
      let x_46 : i32 = ubo.KernelSize;
      if ((x_44 < x_46)) {
      } else {
        break;
      }
      let x_60 : vec2<f32> = f_UV;
      let x_61 : vec2<f32> = dir;
      let x_62 : i32 = i;
      let x_63 : i32 = halfSize;
      let x_69 : vec4<f32> = textureSample(SrcTex, SamplerSrcTex, (x_60 + (x_61 * f32((x_62 - x_63)))));
      let x_77 : i32 = i;
      let x_80 : f32 = kbo.Kernal[x_77].el;
      let x_82 : vec3<f32> = col;
      col = (x_82 + (vec3<f32>(x_69.x, x_69.y, x_69.z) * x_80));

      continuing {
        let x_84 : i32 = i;
        i = (x_84 + 1i);
      }
    }
  } else {
    let x_90 : vec2<f32> = f_UV;
    let x_91 : vec4<f32> = textureSample(SrcTex, SamplerSrcTex, x_90);
    col = vec3<f32>(x_91.x, x_91.y, x_91.z);
  }
  let x_95 : vec3<f32> = col;
  outColor = vec4<f32>(x_95.x, x_95.y, x_95.z, 1.0f);
  return;
}

struct main_out {
  @location(0)
  outColor_1 : vec4<f32>,
}

@fragment
fn main(@location(0) f_UV_param : vec2<f32>) -> main_out {
  f_UV = f_UV_param;
  main_1();
  return main_out(outColor);
}
