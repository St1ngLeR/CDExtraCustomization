#include "envmap_base.fx"

float4 FlipFlopColor1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 FlipFlopColor2 = float4(0.0f, 0.0f, 0.0f, 0.0f);

float FlipFlopAngleInfluence = 1.0; // 0 = constant midpoint pigment; 1 = pigment changes with stock Fresnel weight.

VERTEX VS_FlipFlop(float4 P : POSITION0, float3 N : NORMAL0,
                   float2 tc : TEXCOORD0, float4 col : COLOR0)
{
  VERTEX Out = (VERTEX) 0;

  float3 pWorld = mul(P, TMWorld);
  float3x3 m = TMWorld;
  float3 nWorld = normalize(mul(N, m));
  float3 camray = normalize(C - pWorld);
  float3 r = mul(reflect(camray, nWorld), TMSunRot);

  float StrengthDot = dot(camray, nWorld);
  if (StrengthDot > 0) StrengthDot = 0;
  float CurrStrength = MinStrength + StrengthDiv * min(Fullstrength,
                              pow(1 + StrengthDot, FresnelTerm));
  if (SunState < 1.0) {
    CurrStrength *= 1 - 0.6 * (1 - SunState)
                    * pow(clamp(dot(r, SunDir), 0, 9), 5);
  }

  col.a = CurrStrength;
  Out.Tex1.x = TextureTM._11 * r.x + TextureTM._41;
  Out.Tex1.y = TextureTM._22 * r.y + TextureTM._42;
  Out.Tex2.x = TextureTM2._11 * r.x + TextureTM2._41;
  Out.Tex2.y = TextureTM2._22 * r.y + TextureTM2._42;
  Out.Tex0 = float3(tc, 1);
  Out.Pos = mul(P, TM);
  Out.Color = CalcSunLight(float4(nWorld, 0), col.x, col.a);

  float angle = 2.5 * saturate(0.5 + (CurrStrength - 0.5) * saturate(FlipFlopAngleInfluence));
  float3 pigment = lerp(FlipFlopColor1.rgb, FlipFlopColor2.rgb, angle);
  Out.Color.r = Out.Color.r * pigment.r;
  Out.Color.g = Out.Color.g * pigment.g;
  Out.Color.b = Out.Color.b * pigment.b;

  return Out;
}

technique tec0
{
  pass p0
  {
    VertexShader = compile vs_1_0 VS_FlipFlop();
    PixelShader  = compile ps_1_0 PS_EnvMap();
  }
}
