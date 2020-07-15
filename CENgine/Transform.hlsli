cbuffer TransformCBuf : register(b0)    // Bind to register slot 0
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};